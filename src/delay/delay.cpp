#include "./delay.h"
#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
//初始化时钟
void delay_setup(void)
{
	//选择时钟源
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	
}

void delay_us(uint16_t us) {
	delay_setup();
	//这里应跟据所选时钟源 来确定计数数值
    systick_set_reload(72);
	systick_clear();
	systick_interrupt_enable();
	systick_counter_enable();
	for (int i = 0; i < us; i++)
	{
		while (systick_get_countflag() == 0){

		}
	}
	systick_interrupt_disable();
	systick_counter_disable();
}



void delay_ms(uint16_t ms) {
	delay_setup();
    systick_set_reload(72000);
	systick_clear();
	systick_interrupt_enable();
	systick_counter_enable();
	for (int i = 0; i < ms; i++)
	{
		while (systick_get_countflag() ==0){

		}
	}
	systick_interrupt_disable();
	systick_counter_disable();
}