
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

int main(void){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
	//nvic_enable_irq(NVIC_TIM2_IRQ);
    usart_setup();
	delay_ms(1000);
	printf("main 温度 \r\n");
	TestDHT11();
	//TestBeep();
	//TestKey();
	//TestMCO();
	//TestExti();
}

void TestExti(){
	EXTI_Key_Config();
	
	while (1)
    {
		gpio_toggle(GPIOA,GPIO8);
		delay_ms(5000);
		gpio_toggle(GPIOA,GPIO8);
		delay_ms(5000);
	}
}
void TestMCO(){

	MCO_GPIO_Config();
	rcc_set_mco(RCC_CFGR_MCO_HSE);
	while (1)
    {

	
		gpio_toggle(GPIOC,GPIO2);
		delay_ms(3000);
		gpio_toggle(GPIOC,GPIO2);
		delay_ms(3000);
	}
}


void TestKey(){
	Key_GPIO_Config();
	
	while (1)
    {
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON){
			gpio_toggle(EFFECT_GPIO_PORT,EFFECT_GPIO_PIN);
		}
	}
}
/**
*测试蜂鸣器
**/
void TestBeep(void){
	BEEP_GPIO_Config();
	//gpio_set_mode(GPIOC,GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO2);
	while (1)
    {

		BEEP(ON);
		gpio_toggle(GPIOC,GPIO2);
		delay_ms(3000);
		BEEP(OFF);
		gpio_toggle(GPIOC,GPIO2);
		delay_ms(3000);
	}

}


void TestDHT11(void){
	uint8_t tempetature,humidity;
	rcc_periph_clock_enable(GPIOA);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
				  GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);
	while (1)
    {
        DHT11_Read_Data(&tempetature,&humidity);
        printf("温度： %d \r\n 湿度3: %d  \r\n",tempetature,humidity);
		gpio_toggle(GPIOA,GPIO8);

    }
}