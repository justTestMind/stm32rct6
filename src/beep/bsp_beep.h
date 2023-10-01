#ifndef __BEEP_H
#define __BEEP_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>

#define BEEP_GPIO_PORT GPIOC
#define BEEP_GPIO_CLK RCC_GPIOC
#define BEEP_GPIO_PIN GPIO1
#define ON 1
#define OFF 0
void BEEP_GPIO_Config(void);
void BEEP(int status);
void TestBeep(void);
#endif