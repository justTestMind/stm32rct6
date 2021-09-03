#ifndef __EXTI_H
#define __EXTI_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>

#define KEY2_GPIO_PORT GPIOC
#define KEY2_GPIO_CLK RCC_GPIOC
#define KEY2_GPIO_PIN GPIO3

void EXTI_Key_Config(void);

#endif