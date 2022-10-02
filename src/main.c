
#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <stdio.h>
#include <errno.h>
#include <libopencm3/stm32/timer.h>
#include "./delay.h"
#include "./dht11.h"
#include "./usart.h"
#include "./bsp_beep.h"
#include "./bsp_key.h"
#include "./bsp_mcooutput.h"
#include "./bsp_exti.h"
#include "./oled.h"
#include "./bmp.h"
#include "./rc522/rc522.h"

#include "./usart_dma/usart_dma.h"

int main(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	rcc_periph_clock_enable(RCC_SPI1);
	// nvic_enable_irq(NVIC_TIM2_IRQ);
	// rcc_set_rtc_clock_source(RCC_PLL);

	usart_setup();
	delay_ms(1000);
	// TestDHT11();
	//  TestBeep();
	//  TestKey();
	//  TestMCO();
	//  TestExti();
	// TestOLED();
	//test_usart();
	//test_usart_dma();
	testrc522();
}

void testrc522(void){
	//https://github.com/acoolbest/SPI_RC522_STM32/blob/master/User/main.c
	uint8_t i;
	uint8_t Card_Type1[2];
	uint8_t Card_ID[4];
	uint8_t Card_KEY[6] = {0xff,0xff,0xff,0xff,0xff,0xff};    //{0x11,0x11,0x11,0x11,0x11,0x11};   //密码
	uint8_t Card_Data[16];
	uint8_t status;
	uint8_t addr;
	printf("要开始读卡了 \r\n");
	PcdReset();
	PcdAntennaOff();
	delay_ms(2);
	PcdAntennaOn();

	while(1)
	{
		printf("要开始读卡了111  \r\n");
		delay_ms(1000);
		if(MI_OK==PcdRequest(PICC_REQALL, Card_Type1))
		{
			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];
			//sprintf(print_buf, "Card Type(0x%04X):",cardType);
			printf("Card Type(0x%04X):",cardType);
			switch(cardType){
			case 0x4400:
					printf("Mifare UltraLight \r\n");
					break;
			case 0x0400:
					printf("Mifare One(S50) \r\n");
					break;
			case 0x0200:
					printf("Mifare One(S70) \r\n");
					break;
			case 0x0800:
					printf("Mifare Pro(X) \r\n");
					break;
			case 0x4403:
					printf("Mifare DESFire \r\n");
					break;
			default:
					printf("Unknown Card \r\n");
					continue;
			}
			//delay_ms(10);
			status = PcdAnticoll(Card_ID);//防冲撞
			if(status != MI_OK){
					printf("Anticoll Error\n\r");
					continue;
			}else{
					printf("Serial Number:%02X%02X%02X%02X\n\r",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			}
		}
	}

	

}

void test_usart(void)
{
	uint8_t tempetature, humidity;
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);
	
	while (1)
	{

		printf("hello world \r\n");
		// TestDHT11();
		delay_ms(1000);
		char t = usart_recv_blocking(USART1);
		// if (t == 49)
		// {
		// 	gpio_clear(GPIOA, GPIO8);
		// }
		// else if (t == 50)
		// {
		// 	gpio_set(GPIOA, GPIO8);
		// }

		switch (t)
		{
			case '1':
				gpio_set(GPIOA, GPIO8);
				break;
			case '2':
				gpio_clear(GPIOA, GPIO8);
				break;
			default:
				break;
		}

		printf("rct6 收到指令 %x \r\n", t);
	}
}

void TestOLED()
{

	uint16_t t = ' ';
	delay_setup();
	OLED_Init();
	OLED_ColorTurn(0);	 // 0正常显示, 1反色显示
	OLED_DisplayTurn(0); // 0正常显示 1 屏幕翻转显示
	while (1)
	{
		OLED_ShowPicture(0, 0, 128, 64, BMP1, 1);
		OLED_Refresh();
		delay_ms(3000);
		OLED_ShowPicture(0, 0, 128, 64, BMP2, 1);
		OLED_Refresh();
		delay_ms(3000);
		OLED_Clear();
		OLED_ShowChinese(0, 0, 0, 16, 1);	//中
		OLED_ShowChinese(18, 0, 1, 16, 1);	//景
		OLED_ShowChinese(36, 0, 2, 16, 1);	//园
		OLED_ShowChinese(54, 0, 3, 16, 1);	//电
		OLED_ShowChinese(72, 0, 4, 16, 1);	//子
		OLED_ShowChinese(90, 0, 5, 16, 1);	//技
		OLED_ShowChinese(108, 0, 6, 16, 1); //术
		OLED_ShowString(8, 16, "ZHONGJINGYUAN", 16, 1);
		OLED_ShowString(20, 32, "2014/05/01", 16, 1);
		OLED_ShowString(0, 48, "ASCII:", 16, 1);
		OLED_ShowString(63, 48, "CODE:", 16, 1);
		OLED_ShowChar(48, 48, t, 16, 1); //显示ASCII字符
		t++;
		if (t > '~')
			t = ' ';
		OLED_ShowNum(103, 48, t, 3, 16, 1);
		OLED_Refresh();
		delay_ms(5000);
		OLED_Clear();
		OLED_ShowChinese(0, 0, 0, 16, 1);	// 16*16 中
		OLED_ShowChinese(16, 0, 0, 24, 1);	// 24*24 中
		OLED_ShowChinese(24, 20, 0, 32, 1); // 32*32 中
		OLED_ShowChinese(64, 0, 0, 64, 1);	// 64*64 中
		OLED_Refresh();
		delay_ms(5000);
		OLED_Clear();
		OLED_ShowString(0, 0, "ABC", 8, 1);	  // 6*8 "ABC"
		OLED_ShowString(0, 8, "ABC", 12, 1);  // 6*12 "ABC"
		OLED_ShowString(0, 20, "ABC", 16, 1); // 8*16 "ABC"
		OLED_ShowString(0, 36, "ABC", 24, 1); // 12*24 "ABC"
		OLED_Refresh();
		delay_ms(5000);
		OLED_ScrollDisplay(11, 4, 1);
	}
}

void TestExti()
{
	EXTI_Key_Config();

	while (1)
	{
		gpio_toggle(GPIOA, GPIO8);
		delay_ms(5000);
		gpio_toggle(GPIOA, GPIO8);
		delay_ms(5000);
	}
}
void TestMCO()
{

	MCO_GPIO_Config();
	rcc_set_mco(RCC_CFGR_MCO_HSE);
	while (1)
	{

		gpio_toggle(GPIOC, GPIO2);
		delay_ms(3000);
		gpio_toggle(GPIOC, GPIO2);
		delay_ms(3000);
	}
}

void TestKey()
{
	Key_GPIO_Config();

	while (1)
	{
		if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
		{
			gpio_toggle(EFFECT_GPIO_PORT, EFFECT_GPIO_PIN);
		}
	}
}
/**
 *测试蜂鸣器
 **/
void TestBeep(void)
{
	BEEP_GPIO_Config();
	// gpio_set_mode(GPIOC,GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO2);
	while (1)
	{

		BEEP(ON);
		gpio_toggle(GPIOC, GPIO2);
		delay_ms(3000);
		BEEP(OFF);
		gpio_toggle(GPIOC, GPIO2);
		delay_ms(3000);
	}
}

void TestDHT11(void)
{
	uint8_t tempetature, humidity;

	while (1)
	{
		DHT11_Read_Data(&tempetature, &humidity);
		printf("温度： %d\t 湿度: %d\n", tempetature, humidity);
	}
}