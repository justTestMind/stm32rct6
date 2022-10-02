#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>


#define RC522_RESET_PIN                 GPIO0  //  RESET
#define RC522_RESET_GPIO_PORT           GPIOB  /* GPIOB */
#define RC522_RESET_GPIO_CLK            RCC_GPIOB 
#define RC522_RESET_SET()               gpio_set(RC522_RESET_GPIO_PORT,RC522_RESET_PIN) 
#define RC522_RESET_CLEAR()             gpio_clear(RC522_RESET_GPIO_PORT,RC522_RESET_PIN)

#define RC522_ENABLE                    gpio_clear(GPIOA,GPIO4)
#define RC522_DISABLE                   gpio_set(GPIOA,GPIO4)






/////////////////////////////////////////////////////////////////////
//MF522命令字
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //取消当前命令
#define PCD_AUTHENT           0x0E               //验证密钥
#define PCD_RECEIVE           0x08               //接收数据
#define PCD_TRANSMIT          0x04               //发送数据
#define PCD_TRANSCEIVE        0x0C               //发送并接收数据
#define PCD_RESETPHASE        0x0F               //复位
#define PCD_CALCCRC           0x03               //CRC计算

/////////////////////////////////////////////////////////////////////
//Mifare_One卡片命令字
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //寻天线区内未进入休眠状态
#define PICC_REQALL           0x52               //寻天线区内全部卡
#define PICC_ANTICOLL1        0x93               //防冲撞
#define PICC_ANTICOLL2        0x95               //防冲撞
#define PICC_AUTHENT1A        0x60               //验证A密钥
#define PICC_AUTHENT1B        0x61               //验证B密钥
#define PICC_READ             0x30               //读块
#define PICC_WRITE            0xA0               //写块
#define PICC_DECREMENT        0xC0               //扣款
#define PICC_INCREMENT        0xC1               //充值
#define PICC_RESTORE          0xC2               //调块数据到缓冲区
#define PICC_TRANSFER         0xB0               //保存缓冲区中数据
#define PICC_HALT             0x50               //休眠

/////////////////////////////////////////////////////////////////////
//MF522 FIFO长度定义
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

/////////////////////////////////////////////////////////////////////
//MF522寄存器定义
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F                 0x3F

/////////////////////////////////////////////////////////////////////
//和MF522通讯时返回的错误代码
/////////////////////////////////////////////////////////////////////
#define MI_OK                     0
#define MI_NOTAGERR               1 //(-1)
#define MI_ERR                    2 //(-2)

#define MAXRLEN                   18

void RC522_IO_Init(void);



char PcdRequest(uint8_t req_code,uint8_t *pTagType);
char PcdAnticoll(uint8_t *pSnr);
char PcdSelect(uint8_t *pSnr);
char PcdAuthState(uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr);
char PcdRead(uint8_t addr,uint8_t *pData);
char PcdWrite(uint8_t addr,uint8_t *pData);
char PcdValue(uint8_t dd_mode,uint8_t addr,uint8_t *pValue);
char PcdBakValue(uint8_t sourceaddr, uint8_t goaladdr);
char PcdHalt(void);
void CalulateCRC(uint8_t *pIndata,uint8_t len,uint8_t *pOutData);
char PcdReset(void);
uint8_t ReadRawRC(uint8_t Address);
void WriteRawRC(uint8_t Address, uint8_t value);
void SetBitMask(uint8_t reg,uint8_t mask);
void ClearBitMask(uint8_t reg,uint8_t mask);
char PcdComMF522(uint8_t Command, 
					uint8_t *pInData, 
					uint8_t InLenByte,
					uint8_t *pOutData, 
					uint32_t  *pOutLenBit);
void PcdAntennaOn(void);
void PcdAntennaOff(void);
void RC522_Config(uint8_t Card_Type);

//写0块
/*
 * 終於讓我找到一篇，他的流程是這樣：
 * Sent bits: 26 (7 bits) //尋卡 0x26 / 0x52 都可以
 * Received bits: 04 00
 * Sent bits: 93 20 //防衝撞
 * Received bits: 01 23 45 67 00
 * Sent bits: 93 70 01 23 45 67 00 d0 6f //選卡
 * Received bits: 08 b6 dd （SAK）
 * 不可以認證密鑰，不然後門打不開，好，重點來了，
 * Sent bits: 50 00 57 cd //休眠，50 00 就是 PcdHalt()
 * Sent bits: 40 (7 bits) （特殊指令）//開後門第一條指令，要設定 BitFramingReg 使 傳送 7 個位元，必須要 7 個
 * Received bits: a (4 bits)
 * Sent bits: 43 （特殊指令）//開後門第二條指令
 * Received bits: 0a
 * Sent bits: a0 00 5f b1 //正常的寫區塊第一次交握
 * Received bits: 0a
 * Sent bits: 00 dc 44 20 b8 08 04 00 46 59 25 58 49 10 23 02 c0 10 //正常的寫 block 0 資料
 * Received bits: 0a
 * 重點就是要：
 * 1. 不可以進行 3 Pass Authenticaiton 
 * 2. 發 PcdHalt() 
 * 3. 發 0x40 in 7-bit 
 * 4. 發 0x43
 */
 
/*
 * 解释说明
 * 首先安照正常S50卡的操作顺序进行 寻卡->防冲撞->选卡 ，
 * 接下来不要验证密码，然后 休眠。
 *
 * 下面发送第一条特殊指令，他是16进制的0x40，必须只发送7位,
 * 很多朋友就是不清楚这里如何发送0x40以及如何只发送7位，只需要调用下面两个函数就可以了。
 * Write_MFRC522()或WriteRawRC()
 * MFRC522_ToCard()或PcdComMF522()
 * 第一个是设置发送7个位，需要设置BitFramingReg为0x07，
 * 第二个是写指令函数，不要说你不会调用这两函数，不会的就要好好看下程序中是怎样用到这两函数的。
 *
 * 然后发送第二条指令0x43，要发8位，设置BitFramingReg为0x00（好像不设置这个也可以，默认的8位），
 *
 * 接下来发送buf[ ]={0xa0,0x00, 0x5f, 0xb1}，
 * 同样用MFRC522_ToCard()函数
 *（这串指令我也没懂什么意思，就当做是特殊指令吧，如果有人理解这个可以给我说下），
 * 这里发送的指令都会应答0x0a（我的好像是0xa0，或许是大小端问题吧），
 * 如果没收到说明没有弄对，建议一个一个测试。
 * 通过上述特殊指令后就是写块0了，
 * 奇怪的是按照上面说的正常写块0，我是没有成功（或许这样是可以的，我看原理都是一样的），
 * 我还是单独调用的MFRC522_ToCard()函数写块0，上面是18个字节，但是正常的块0是16字节，
 *
 * 所以这里有个重点，先把要复制的卡的块0字节读出，
 * 用CalulateCRC()函数计算出2位校验位放在16字节后面组成18字节一起写入到块0，
 * 不可以随便填写块0的数据，因为有校验。到此，块0就可以成功写入
 */

/*
 * https://blog.csdn.net/baidu_34570497/article/details/79689778
 * http://www.51hei.com/bbs/dpj-85337-1.html
 */
