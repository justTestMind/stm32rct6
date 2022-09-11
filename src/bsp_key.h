#ifndef __Key_H
#define __Key_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>
//按键相关配置
#define KEY1_GPIO_PORT GPIOC
#define KEY1_GPIO_CLK RCC_GPIOC
#define KEY1_GPIO_PIN GPIO9
#define KEY_ON 1
#define KEY_OFF 0


//按键影响端口相关配置
#define EFFECT_GPIO_PORT GPIOC
#define EFFECT_GPIO_CLK RCC_GPIOC
#define EFFECT_GPIO_PIN GPIO13



void Key_GPIO_Config(void);
uint8_t Key_Scan(uint_fast32_t GPIOx,uint16_t GPIO_Pin);
#endif