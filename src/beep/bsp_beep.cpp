
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <errno.h>
#include "./bsp_beep.h"
#include "../delay/delay.h"


void BEEP_GPIO_Config(void)
{
    rcc_periph_clock_enable(BEEP_GPIO_CLK);
	gpio_set_mode(BEEP_GPIO_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, BEEP_GPIO_PIN);
	gpio_clear(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}
void BEEP(int status)
{
	//
	if(status > 0){
		printf("输出高电平 不响\r\n");
		gpio_set(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
	}else{
		printf("输出低电平 响\r\n");
		gpio_clear(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
	}
}



// /**
//  *测试蜂鸣器
//  **/
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


