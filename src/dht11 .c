#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include "./dht11.h"
#include "./delay.h"
#include <stdio.h>

uint16_t DHT11_DQ_IN(bool print)
{
    int32_t data = gpio_get(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
    if (print)
    {
        printf("GPIO 电平情况  %d \r\n", data);
        printf("DHT11_GPIO_PIN 的值   %d \r\n", DHT11_GPIO_PIN);
    }

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
    printf("dht11 重置 \r\n");
    
    rcc_periph_clock_enable(DHT11_GPIO_CLK);
    DHT11_IO_OUT();
    gpio_set(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
    printf("越过不稳定状态 %d \r\n", DHT11_DQ_IN(true));
    delay_ms(3000);
    gpio_clear(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
    printf("主机发开始信号 %d \r\n", DHT11_DQ_IN(true));
    delay_ms(28);
    // delay_ms(3000);
    gpio_set(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
    printf("主机拉高 %d \r\n", DHT11_DQ_IN(true));
    delay_us(30);
    //      delay_ms(3000);
    //     printf("准备切换输入模式");
    //      DHT11_IO_IN();
    //  printf("输入模式中取值 %d \r\n",DHT11_DQ_IN(true));

    //     delay_ms(3000);
}
//接收
uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;
    DHT11_IO_IN();
    printf("当前电平信息 %d \r\n", DHT11_DQ_IN(true));

    while (DHT11_DQ_IN(false) && retry < 100)
    {
        retry++;
        delay_us(1);
    }
    if (retry >= 100)
    {
        printf("期望dht响应信号  当前电平信息 %d \r\n", DHT11_DQ_IN(false));
        return 1;
    }

    printf(" %d us 后收到dht响应信号;当前电平信息 %d\r\n", retry, DHT11_DQ_IN(false));
    retry = 0;
    while (!DHT11_DQ_IN(false) && retry < 100)
    {
        retry++;
        delay_us(1);
    }

    if (retry >= 100)
    {
        printf("期望Dht拉高  当前电平信息 %d \r\n", DHT11_DQ_IN(false));
        return 1;
    }

    return 0;
}

uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;
    while (DHT11_DQ_IN(false) && retry < 100)
    {
        retry++;
        delay_us(1);
    }

    retry = 0;
    while (!DHT11_DQ_IN(false) && retry < 100)
    {
        retry++;
        delay_us(1);
    }
    delay_us(40);
    if (DHT11_DQ_IN(false))
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
    printf("当前 dat %d \r\n", dat);

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
