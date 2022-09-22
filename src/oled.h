#ifndef __OLED_H
#define __OLED_H 

#include <libopencm3/stm32/gpio.h>

//oled 端口定义
#define OLED_SCL_RCC RCC_GPIOA
#define OLED_SCL_GPIO GPIOA
#define OLED_SCL_GPIO_PIN GPIO5

#define OLED_SDA_RCC RCC_GPIOA
#define OLED_SDA_GPIO GPIOA
#define OLED_SDA_GPIO_PIN GPIO7

#define OLED_SCL_Clr() gpio_clear(OLED_SCL_GPIO,OLED_SCL_GPIO_PIN)//SCL
#define OLED_SCL_Set() gpio_set(OLED_SCL_GPIO,OLED_SCL_GPIO_PIN)

#define OLED_SDA_Clr() gpio_clear(OLED_SDA_GPIO,OLED_SDA_GPIO_PIN)//DIN
#define OLED_SDA_Set() gpio_set(OLED_SDA_GPIO,OLED_SDA_GPIO_PIN)

#define OLED_RES_Clr() gpio_clear(GPIOD,GPIO2)//RES
#define OLED_RES_Set() gpio_set(GPIOD,GPIO2)


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_ClearPoint(uint16_t x,uint16_t y);
void OLED_ColorTurn(uint16_t i);
void OLED_DisplayTurn(uint16_t i);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_WaitAck(void);
void Send_Byte(uint16_t dat);
void OLED_WR_Byte(uint16_t dat,uint16_t mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint16_t x,uint16_t y,uint16_t t);
void OLED_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t mode);
void OLED_DrawCircle(uint16_t x,uint16_t y,uint16_t r);
void OLED_ShowChar(uint16_t x,uint16_t y,uint8_t chr,uint16_t size1,uint16_t mode);
void OLED_ShowChar6x8(uint16_t x,uint16_t y,uint16_t chr,uint16_t mode);
void OLED_ShowString(uint16_t x,uint16_t y,uint8_t *chr,uint16_t size1,uint16_t mode);
void OLED_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint16_t len,uint16_t size1,uint16_t mode);
void OLED_ShowChinese(uint16_t x,uint16_t y,uint16_t num,uint16_t size1,uint16_t mode);
void OLED_ScrollDisplay(uint16_t num,uint16_t space,uint16_t mode);
void OLED_ShowPicture(uint16_t x,uint16_t y,uint16_t sizex,uint16_t sizey,uint8_t BMP[],uint16_t mode);
void OLED_Init(void);

#endif

