
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <errno.h>
#include "./bsp_mcooutput.h"

void MCO_GPIO_Config(void)
{
    rcc_periph_clock_enable(MCO_GPIO_CLK);
	gpio_set_mode(MCO_GPIO_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, MCO_GPIO_PIN);
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