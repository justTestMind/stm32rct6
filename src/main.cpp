
#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <stdio.h>
#include <errno.h>
#include <libopencm3/stm32/timer.h>
#include "./delay/delay.h"
#include "./dht11/dht11.h"
#include "./usart/usart.h"
#include "./beep/bsp_beep.h"
#include "./key/bsp_key.h"
#include "./bsp_mcooutput.h"
#include "./exti/bsp_exti.h"
#include "./oled/oled.h"



#include "./usart_dma/usart_dma.h"

#include "./bluetooth/blue_tooth.h"

#include "./rc522/rc522.h"

void testrc522(void){
	//https://github.com/acoolbest/SPI_RC522_STM32/blob/master/User/main.c
	uint8_t i;
	uint8_t Card_Type1[2];
	uint8_t Card_ID[4];
	uint8_t Card_KEY[6] = {0xff,0xff,0xff,0xff,0xff,0xff};    //{0x11,0x11,0x11,0x11,0x11,0x11};   //密码
	uint8_t Card_Data[16];
	uint8_t status;
	uint8_t addr;
	RC522_IO_Init();
	usart_print("要开始读卡了 \r\n");
	PcdReset();
	PcdAntennaOff();
	delay_ms(2);
	PcdAntennaOn();

	while(1)
	{
		usart_print("要开始读卡了111 %d \r\n");
		delay_ms(1000);
		if(MI_OK==PcdRequest(PICC_REQALL, Card_Type1))
		{
			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];
			//sprintf(print_buf, "Card Type(0x%04X):",cardType);
			//printf("Card Type(0x%04X):",cardType);
			switch(cardType){
			case 0x4400:
					usart_print("Mifare UltraLight \r\n");
					break;
			case 0x0400:
					usart_print("Mifare One(S50) \r\n");
					break;
			case 0x0200:
					usart_print("Mifare One(S70) \r\n");
					break;
			case 0x0800:
					usart_print("Mifare Pro(X) \r\n");
					break;
			case 0x4403:
					usart_print("Mifare DESFire \r\n");
					break;
			default:
					usart_print("Unknown Card \r\n");
					continue;
			}
			//delay_ms(10);
			status = PcdAnticoll(Card_ID);//防冲撞
			if(status != MI_OK){
					usart_print("Anticoll Error\n\r");
					continue;
			}else{
					printf("Serial Number:%02X%02X%02X%02X\n\r",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			}
		}
	}

	

}

int main(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	// rcc_periph_clock_enable(RCC_SPI1);
	//  nvic_enable_irq(NVIC_TIM2_IRQ);
	//  rcc_set_rtc_clock_source(RCC_PLL);

	usart_setup();
	delay_ms(1000);
	// TestDHT11();
	//  TestBeep();
	//TestKey();
	//testrc522();
	//  TestMCO();
	 // TestExti();

	TestOLED();
	// test_usart();
	// test_usart_dma();
	 //testrc522();

	// test_blur_tooth();
	//rcc_periph_clock_enable(rcc_apb)
	// rcc_periph_clock_enable(RCC_GPIOA);
	// gpio_set_mode(GPIOA,GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO8);
	
	
}



// /**
//  *测试蜂鸣器
//  **/
// void TestBeep(void)
// {
// 	BEEP_GPIO_Config();
// 	// gpio_set_mode(GPIOC,GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO2);
// 	while (1)
// 	{

// 		BEEP(ON);
// 		gpio_toggle(GPIOC, GPIO2);
// 		delay_ms(3000);
// 		BEEP(OFF);
// 		gpio_toggle(GPIOC, GPIO2);
// 		delay_ms(3000);
// 	}
// }