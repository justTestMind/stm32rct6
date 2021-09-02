#ifndef __Key_H
#define __Key_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>

#define KEY1_GPIO_PORT GPIOA
#define KEY1_GPIO_CLK RCC_GPIOA
#define KEY1_GPIO_PIN GPIO0
#define KEY_ON 1
#define KEY_OFF 0
void Key_GPIO_Config(void);
uint8_t Key_Scan(uint_fast32_t GPIOx,uint16_t GPIO_Pin);
#endif