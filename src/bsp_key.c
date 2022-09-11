
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <errno.h>
#include "./bsp_key.h"

void Key_GPIO_Config(void)
{
	/* Setup GPIO pin GPIO_USART1_TX. */
	rcc_periph_clock_enable(KEY1_GPIO_CLK);
	gpio_set_mode(KEY1_GPIO_PORT, GPIO_MODE_INPUT,
				  GPIO_CNF_INPUT_FLOAT, KEY1_GPIO_PIN);
	// gpio_clear(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
	rcc_periph_clock_enable(EFFECT_GPIO_CLK);

	gpio_set_mode(EFFECT_GPIO_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, EFFECT_GPIO_PIN);
}
uint8_t Key_Scan(uint_fast32_t GPIOx, uint16_t GPIO_Pin)
{
	if (gpio_get(GPIOx, GPIO_Pin) > 0)
	{
		while (gpio_get(GPIOx, GPIO_Pin) > 0)
		{
		}
		printf(" 按键按下后 松开了\r\n");
		return KEY_ON;
	}

	return KEY_OFF;
}
