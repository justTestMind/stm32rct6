#ifndef __DHT11_H
#define __DHT11_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>

#define DHT11_GPIO_PORT GPIOA
#define DHT11_GPIO_CLK RCC_GPIOA
#define DHT11_GPIO_PIN GPIO3


uint8_t DHT11_Init(void);
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Check(void);
void DHT11_Rst(void);

#endif