
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>

#include "./rc522.h"
#include <libopencm3/stm32/spi.h>

void spi_setup(void)
{

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_SPI1);

	
	/* Configure GPIOs: SS=PA4, SCK=PA5, MISO=PA6 and MOSI=PA7 */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
				  GPIO_CNF_OUTPUT_PUSHPULL, GPIO4 | GPIO5 | GPIO7);

	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO6);
	gpio_set(GPIOA, GPIO6);

	/* Reset SPI, SPI_CR1 register cleared, SPI is disabled */
	spi_reset(SPI1);

	/* Set up SPI in Master mode with:
	 * Clock baud rate: 1/64 of peripheral clock frequency
	 * Clock polarity: Idle High
	 * Clock phase: Data valid on 2nd clock pulse
	 * Data frame format: 8-bit
	 * Frame format: MSB First
	 */
	spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_8, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
					SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);

	/*
	 * Set NSS management to software.
	 *
	 * Note:
	 * Setting nss high is very important, even if we are controlling the GPIO
	 * ourselves this bit needs to be at least set to 1, otherwise the spi
	 * peripheral will not send any data out.
	 */
	spi_enable_software_slave_management(SPI1);
	spi_set_nss_high(SPI1);

	/* Enable SPI1 periph. */
	spi_enable(SPI1);
}

void RC522_IO_Init(void)
{

	spi_setup();
	// SPI_Configuration(SPI1);

	rcc_periph_clock_enable(RC522_RESET_GPIO_CLK);

	gpio_set_mode(RC522_RESET_GPIO_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, RC522_RESET_PIN);

}

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(uint8_t req_code, uint8_t *pTagType)
{
	char status;
	uint32_t unLen;
	uint8_t ucComMF522Buf[MAXRLEN];

	ClearBitMask(Status2Reg, 0x08);	 //清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
	WriteRawRC(BitFramingReg, 0x07); //发送的最后一个字节的 七位
	SetBitMask(TxControlReg, 0x03);	 // TX1,TX2管脚的输出信号传递经发送调制的13.56的能量载波信号

	ucComMF522Buf[0] = req_code; //存入 卡片命令字

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen); //寻卡
	//   UART_send_byte(status);
	if ((status == MI_OK) && (unLen == 0x10)) //寻卡成功返回卡类型
	{
		*pTagType = ucComMF522Buf[0];
		*(pTagType + 1) = ucComMF522Buf[1];
	}
	else
	{
		status = MI_ERR;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAnticoll(uint8_t *pSnr)
{
	char status;
	uint8_t i, snr_check = 0;
	uint32_t unLen;
	uint8_t ucComMF522Buf[MAXRLEN];

	ClearBitMask(Status2Reg, 0x08);	 //清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
	WriteRawRC(BitFramingReg, 0x00); //清理寄存器 停止收发
	ClearBitMask(CollReg, 0x80);	 //清ValuesAfterColl所有接收的位在冲突后被清除

	ucComMF522Buf[0] = PICC_ANTICOLL1; //卡片防冲突命令
	ucComMF522Buf[1] = 0x20;

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, &unLen); //与卡片通信

	if (status == MI_OK)
	{
		for (i = 0; i < 4; i++)
		{
			*(pSnr + i) = ucComMF522Buf[i]; //读出UID
			snr_check ^= ucComMF522Buf[i];
		}
		if (snr_check != ucComMF522Buf[i])
		{
			status = MI_ERR;
		}
	}

	SetBitMask(CollReg, 0x80);
	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(uint8_t *pSnr)
{
	char status;
	uint8_t i;
	uint32_t unLen;
	uint8_t ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = PICC_ANTICOLL1;
	ucComMF522Buf[1] = 0x70;
	ucComMF522Buf[6] = 0;
	for (i = 0; i < 4; i++)
	{
		ucComMF522Buf[i + 2] = *(pSnr + i);
		ucComMF522Buf[6] ^= *(pSnr + i);
	}
	CalulateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]);

	ClearBitMask(Status2Reg, 0x08);

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &unLen);

	if ((status == MI_OK) && (unLen == 0x18))
	{
		status = MI_OK;
	}
	else
	{
		status = MI_ERR;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAuthState(uint8_t auth_mode, uint8_t addr, uint8_t *pKey, uint8_t *pSnr)
{
	char status;
	uint32_t unLen;
	uint8_t i, ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = auth_mode;
	ucComMF522Buf[1] = addr;
	for (i = 0; i < 6; i++)
	{
		ucComMF522Buf[i + 2] = *(pKey + i);
	}
	for (i = 0; i < 6; i++)
	{
		ucComMF522Buf[i + 8] = *(pSnr + i);
	}

	status = PcdComMF522(PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, &unLen);
	if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
	{
		status = MI_ERR;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          p [OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRead(uint8_t addr, uint8_t *pData)
{
	char status;
	uint32_t unLen;
	uint8_t i, ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = PICC_READ;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);
	if ((status == MI_OK) && (unLen == 0x90))
	{
		for (i = 0; i < 16; i++)
		{
			*(pData + i) = ucComMF522Buf[i];
		}
	}
	else
	{
		status = MI_ERR;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          p [IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdWrite(uint8_t addr, uint8_t *pData)
{
	char status;
	uint32_t unLen;
	uint8_t i, ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = PICC_WRITE;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

	if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
	{
		status = MI_ERR;
	}

	if (status == MI_OK)
	{
		for (i = 0; i < 16; i++)
		{
			ucComMF522Buf[i] = *(pData + i);
		}
		CalulateCRC(ucComMF522Buf, 16, &ucComMF522Buf[16]);

		status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, &unLen);
		if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
		{
			status = MI_ERR;
		}
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdValue(uint8_t dd_mode, uint8_t addr, uint8_t *pValue)
{
	char status;
	uint32_t unLen;
	uint8_t i, ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = dd_mode;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

	if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
	{
		status = MI_ERR;
	}

	if (status == MI_OK)
	{
		for (i = 0; i < 16; i++)
		{
			ucComMF522Buf[i] = *(pValue + i);
		}
		CalulateCRC(ucComMF522Buf, 4, &ucComMF522Buf[4]);
		unLen = 0;
		status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 6, ucComMF522Buf, &unLen);
		if (status != MI_ERR)
		{
			status = MI_OK;
		}
	}

	if (status == MI_OK)
	{
		ucComMF522Buf[0] = PICC_TRANSFER;
		ucComMF522Buf[1] = addr;
		CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

		status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

		if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
		{
			status = MI_ERR;
		}
	}
	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(uint8_t sourceaddr, uint8_t goaladdr)
{
	char status;
	uint32_t unLen;
	uint8_t ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = PICC_RESTORE;
	ucComMF522Buf[1] = sourceaddr;
	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

	if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
	{
		status = MI_ERR;
	}

	if (status == MI_OK)
	{
		ucComMF522Buf[0] = 0;
		ucComMF522Buf[1] = 0;
		ucComMF522Buf[2] = 0;
		ucComMF522Buf[3] = 0;
		CalulateCRC(ucComMF522Buf, 4, &ucComMF522Buf[4]);

		status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 6, ucComMF522Buf, &unLen);
		if (status != MI_ERR)
		{
			status = MI_OK;
		}
	}

	if (status != MI_OK)
	{
		return MI_ERR;
	}

	ucComMF522Buf[0] = PICC_TRANSFER;
	ucComMF522Buf[1] = goaladdr;

	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

	if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
	{
		status = MI_ERR;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
	// char status;
	uint32_t unLen;
	uint8_t ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = PICC_HALT;
	ucComMF522Buf[1] = 0;
	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

	// status =
	PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

	return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData)
{
	uint8_t i, n;
	ClearBitMask(DivIrqReg, 0x04);
	WriteRawRC(CommandReg, PCD_IDLE);
	SetBitMask(FIFOLevelReg, 0x80);
	for (i = 0; i < len; i++)
	{
		WriteRawRC(FIFODataReg, *(pIndata + i));
	}
	WriteRawRC(CommandReg, PCD_CALCCRC);
	i = 0xFF;
	do
	{
		n = ReadRawRC(DivIrqReg);
		i--;
	} while ((i != 0) && !(n & 0x04));
	pOutData[0] = ReadRawRC(CRCResultRegL);
	pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{

	RC522_IO_Init();

	RC522_RESET_SET();	 // RST522_1;
	delay_us(500);		 //_NOP();
	RC522_RESET_CLEAR(); // RST522_0;
	delay_ms(60);		 //_NOP();
	RC522_RESET_SET();	 // RST522_1;
	delay_us(500);		 //_NOP();
	WriteRawRC(CommandReg, PCD_RESETPHASE);
	delay_ms(20); //_NOP();

	WriteRawRC(ModeReg, 0x3D);		 //定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
	WriteRawRC(TReloadRegL, 30);	 // 16位定时器低位
	WriteRawRC(TReloadRegH, 0);		 // 16位定时器高位
	WriteRawRC(TModeReg, 0x8D);		 //定义内部定时器的设置
	WriteRawRC(TPrescalerReg, 0x3E); //设置定时器分频系数
	WriteRawRC(TxAutoReg, 0x40);	 //调制发送信号为100%ASK

	ClearBitMask(TestPinEnReg, 0x80); // off MX and DTRQ out
	WriteRawRC(TxAutoReg, 0x40);

	return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//功    能：读RC632寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
uint8_t ReadRawRC(uint8_t Address)
{
	uint8_t ucAddr;
	uint8_t ucResult = 0;
	ucAddr = ((Address << 1) & 0x7E) | 0x80;
	delay_ms(10);
	RC522_ENABLE;
	spi_write(SPI1, ucAddr);
  delay_ms(10);
	// SPI_WriteNBytes(SPI1,&ucAddr,1);
	// SPI_ReadNBytes(SPI1,&ucResult,1);
	ucResult = spi_read(SPI1);
	RC522_DISABLE;
	return ucResult;
}

/////////////////////////////////////////////////////////////////////
//功    能：写RC632寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void WriteRawRC(uint8_t Address, uint8_t value)
{
	uint8_t ucAddr;
	uint8_t write_buffer[2] = {0};
	ucAddr = ((Address << 1) & 0x7E);
	write_buffer[0] = ucAddr;
	write_buffer[1] = value;
	RC522_ENABLE;
	spi_write(SPI1, write_buffer[0]);
	spi_write(SPI1, write_buffer[1]);
	// SPI_WriteNBytes(SPI1,write_buffer,2);
	RC522_DISABLE;
  
  //printf("WriteRawRC \r\n");
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(uint8_t reg, uint8_t mask)
{
	uint8_t tmp = 0x0;
	tmp = ReadRawRC(reg);
	WriteRawRC(reg, tmp | mask); // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(uint8_t reg, uint8_t mask)
{
	uint8_t tmp = 0x0;
	tmp = ReadRawRC(reg);
  	printf("ReadRawRC: %d \r\n",tmp);

	WriteRawRC(reg, tmp & ~mask); // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pIn [IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOut [OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(uint8_t Command,
				 uint8_t *pInData,
				 uint8_t InLenByte,
				 uint8_t *pOutData,
				 uint32_t *pOutLenBit)
{
	char status = MI_ERR;
	uint8_t irqEn = 0x00;
	uint8_t waitFor = 0x00;
	uint8_t lastBits;
	uint8_t n;
	uint32_t i;
	switch (Command)
	{
	case PCD_AUTHENT:	// Mifare认证
		irqEn = 0x12;	//允许错误中断请求ErrIEn  允许空闲中断IdleIEn
		waitFor = 0x10; //认证寻卡等待时候 查询空闲中断标志位
		break;
	case PCD_TRANSCEIVE: //接收发送 发送接收
		irqEn = 0x77;	 //允许TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
		waitFor = 0x30;	 //寻卡等待时候 查询接收中断标志位与 空闲中断标志位
		break;
	default:
		break;
	}

	WriteRawRC(ComIEnReg, irqEn | 0x80); // IRqInv置位管脚IRQ与Status1Reg的IRq位的值相反
	ClearBitMask(ComIrqReg, 0x80);		 // Set1该位清零时，CommIRqReg的屏蔽位清零
	WriteRawRC(CommandReg, PCD_IDLE);	 //写空闲命令
	SetBitMask(FIFOLevelReg, 0x80);		 //置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除

	for (i = 0; i < InLenByte; i++)
	{
		WriteRawRC(FIFODataReg, pInData[i]);
	}								 //写数据进FIFOdata
	WriteRawRC(CommandReg, Command); //写命令

	if (Command == PCD_TRANSCEIVE)
	{
		SetBitMask(BitFramingReg, 0x80);
	} // StartSend置位启动数据发送 该位与收发命令使用时才有效

	i = 800; //根据时钟频率调整，操作M1卡最大等待时间25ms
	do		 //认证 与寻卡等待时间
	{
		n = ReadRawRC(ComIrqReg); //查询事件中断
		i--;
	} while ((i != 0) && !(n & 0x01) && !(n & waitFor)); //退出条件i=0,定时器中断，与写空闲命令
	ClearBitMask(BitFramingReg, 0x80);					 //清理允许StartSend位

	if (i != 0)
	{
		if (!(ReadRawRC(ErrorReg) & 0x1B)) //读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
		{
			status = MI_OK;
			if (n & irqEn & 0x01) //是否发生定时器中断
			{
				status = MI_NOTAGERR;
			}
			if (Command == PCD_TRANSCEIVE)
			{
				n = ReadRawRC(FIFOLevelReg);			 //读FIFO中保存的字节数
				lastBits = ReadRawRC(ControlReg) & 0x07; //最后接收到得字节的有效位数
				if (lastBits)
				{
					*pOutLenBit = (n - 1) * 8 + lastBits;
				} // N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
				else
				{
					*pOutLenBit = n * 8;
				} //最后接收到的字节整个字节有效
				if (n == 0)
				{
					n = 1;
				}
				if (n > MAXRLEN)
				{
					n = MAXRLEN;
				}
				for (i = 0; i < n; i++)
				{
					pOutData[i] = ReadRawRC(FIFODataReg);
				}
			}
		}
		else
		{
			status = MI_ERR;
		}
	}

	SetBitMask(ControlReg, 0x80); // stop timer now
	WriteRawRC(CommandReg, PCD_IDLE);
	return status;
}

/////////////////////////////////////////////////////////////////////
//开启天线
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void)
{
	uint8_t i;
	i = ReadRawRC(TxControlReg);
	if (!(i & 0x03))
	{
		SetBitMask(TxControlReg, 0x03);
	}
}

/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}

/*
 * 函数名：RC522_Config
 * 描述  ：设置RC522的工作方式
 * 输入  ：Card_Type，工作方式
 * 返回  : 无
 * 调用  ：外部调用
 */
void RC522_Config(uint8_t Card_Type)
{
	if (Card_Type == 'A')
	{
		ClearBitMask(Status2Reg, 0x08);
		WriteRawRC(ModeReg, 0x3D);	 // 3F
		WriteRawRC(RxSelReg, 0x86);	 // 84
		WriteRawRC(RFCfgReg, 0x7F);	 // 4F
		WriteRawRC(TReloadRegL, 30); // tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
		WriteRawRC(TReloadRegH, 0);
		WriteRawRC(TModeReg, 0x8D);
		WriteRawRC(TPrescalerReg, 0x3E);
		// WriteRawRC(TxAutoReg,0x40);
		delay_ms(5);
		PcdAntennaOn();
	}
}

/////////////////////////////////////////////////////////////////////
//功    能：写0块，改写厂商信息，复制卡
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
uint8_t PcdWrite0Block(void)
{
	uint8_t Card_Type1[2];
	uint8_t Card_ID[4];

	uint32_t unLen;
	uint8_t ucComMF522Buf[MAXRLEN];
	uint8_t ucComMF522Buf1[MAXRLEN] = {0xa0, 0x00, 0x5f, 0xb1};
	uint8_t ucComMF522Buf2[MAXRLEN] = {0x00, 0xdc, 0x44, 0x20, 0xb8, 0x08, 0x04, 0x00, 0x46, 0x59, 0x25, 0x58, 0x49, 0x10, 0x23, 0x02};

	if (PcdRequest(PICC_REQALL, Card_Type1) != MI_OK)
		return MI_ERR;
	uint16_t cardType = (Card_Type1[0] << 8) | Card_Type1[1];

	if (cardType == 0x4400 || cardType == 0x0400 || cardType == 0x0200 || cardType == 0x0800 || cardType == 0x4403)
	{
		if (PcdAnticoll(Card_ID) != MI_OK)
			return MI_ERR;
		if (PcdSelect(Card_ID) != MI_OK)
			return MI_ERR;
		PcdHalt();

		WriteRawRC(BitFramingReg, 0x07); //发送的最后一个字节的 七位
		ucComMF522Buf[0] = 0x40;
		if (PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen) != MI_OK)
			return MI_ERR;

		WriteRawRC(BitFramingReg, 0x00);
		ucComMF522Buf[0] = 0x43;
		if (PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen) != MI_OK)
			return MI_ERR;

		if (PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf1, 4, ucComMF522Buf1, &unLen) != MI_OK)
			return MI_ERR;

		CalulateCRC(ucComMF522Buf2, 16, &ucComMF522Buf2[16]);
		return PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf2, 18, ucComMF522Buf2, &unLen);
	}

	return MI_ERR;
}
