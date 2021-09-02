
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

int main(void){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(GPIOC,GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO2);
    usart_setup();
	//TestDHT11();
	TestBeep();
	//TestKey();
}

void TestKey(){
	Key_GPIO_Config();
	while (1)
    {
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON){
			gpio_toggle(GPIOC,GPIO2);
		}
	}
}

void TestBeep(void){
	BEEP_GPIO_Config();
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
	DHT11_Init();
	while (1)
    {
        DHT11_Read_Data(&tempetature,&humidity);
        printf("温度： %d \r\n 湿度3: %d  \r\n",tempetature,humidity);
        delay_ms(3000);
		gpio_toggle(GPIOC,GPIO2);

    }
}