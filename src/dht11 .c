#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include "./dht11.h"
#include "./delay.h"
#include <stdio.h>

uint16_t DHT11_DQ_IN()
{
    int32_t data = gpio_get(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
    return (data & DHT11_GPIO_PIN) > 0 ? 1 : 0;
}

//配置为输出模式
void DHT11_IO_OUT(void)
{
    gpio_set_mode(DHT11_GPIO_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, DHT11_GPIO_PIN);
}
//配置为输入模式
void DHT11_IO_IN(void)
{
    gpio_set_mode(DHT11_GPIO_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, DHT11_GPIO_PIN);
}

void DHT11_Rst(void)
{
    rcc_periph_clock_enable(DHT11_GPIO_CLK);
    DHT11_IO_OUT();
    //延时3秒 避免读取太频繁
    gpio_set(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
    delay_ms(3000);
    //主机发送开始信号
    gpio_clear(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
    delay_ms(20);
    //拉高并延时等待
    gpio_set(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
    delay_us(30);
}
//接收
uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;
    DHT11_IO_IN();
    while (DHT11_DQ_IN() && retry < 100)
    {
        retry++;
        delay_us(1);
    }
    if (retry >= 100)
    {
        return 1;
    }
    retry = 0;
    while (!DHT11_DQ_IN() && retry < 100)
    {
        retry++;
        delay_us(1);
    }

    if (retry >= 100)
    {
        return 1;
    }

    return 0;
}

uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;
    while (DHT11_DQ_IN() && retry < 100)
    {
        retry++;
        delay_us(1);
    }

    retry = 0;
    while (!DHT11_DQ_IN() && retry < 100)
    {
        retry++;
        delay_us(1);
    }
    delay_us(40);
    if (DHT11_DQ_IN())
        return 1;
    else
        return 0;
}

uint8_t DHT11_Read_Byte(void)
{
    uint8_t i, dat;
    dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;
    DHT11_Rst();
    if (DHT11_Check() == 0)
    {
        for (i = 0; i < 5; i++)
        {
            buf[i] = DHT11_Read_Byte();
        }
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
    }
    else
    {
        printf("dht11 检测不通过\r\n");
        return 1;
    };
    return 0;
}
