#ifndef __MCOOUTPUT_H
#define __MCOOUTPUT_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>

#define MCO_GPIO_PORT GPIOA
#define MCO_GPIO_CLK RCC_GPIOA
#define MCO_GPIO_PIN GPIO8
void MCO_GPIO_Config(void);
#endif