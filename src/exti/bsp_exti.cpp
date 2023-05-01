
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <errno.h>
#include "./bsp_exti.h"
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include "../usart/usart.h"
#include "../delay/delay.h"

uint16_t exti_line_state;

static void gpio_setup(void)
{
	rcc_periph_clock_enable(KEY2_GPIO_CLK);
	gpio_set_mode(KEY2_GPIO_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, KEY2_GPIO_PIN);
	gpio_clear(KEY2_GPIO_PORT,KEY2_GPIO_PIN);
}

static void exti_setup(void)
{
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Enable AFIO clock. */
	rcc_periph_clock_enable(RCC_AFIO);

	/* Enable EXTI0 interrupt. */
	nvic_enable_irq(NVIC_EXTI0_IRQ);

	/* Set GPIO0 (in GPIO port A) to 'input float'. */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO0);

	/* Configure the EXTI subsystem. */
	exti_select_source(EXTI0, GPIOA);
	exti_set_trigger(EXTI0, EXTI_TRIGGER_BOTH);
	exti_enable_request(EXTI0);
}

void exti0_isr(void)
{
	exti_line_state = GPIOA_IDR;
	printf("输入寄存器的值  %d \r\n",exti_line_state & 1);
    //printf("A 寄存器的状态 ",exti_line_state);
	/* The LED (PC12) is on, but turns off when the button is pressed. */
	if ((exti_line_state & (1 << 0)) != 0) {
		printf("开始亮灯 \r\n");
		gpio_clear(KEY2_GPIO_PORT, KEY2_GPIO_PIN);
	} else {
		gpio_set(KEY2_GPIO_PORT, KEY2_GPIO_PIN);
	}

	exti_reset_request(EXTI0);
}

void EXTI_Key_Config(void)
{
    gpio_setup();
	exti_setup();

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



