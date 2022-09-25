#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/nvic.h>
#include "../delay.h"
#include "./usart.h"
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
void dma_write(char *data, int size)
{
    /*
     * 串口1 发送使用通道 4
     */
    /* Reset DMA channel*/

    dma_channel_reset(DMA1, DMA_CHANNEL4);

    dma_set_peripheral_address(DMA1, DMA_CHANNEL4, (uint32_t)&USART1_DR);
    dma_set_memory_address(DMA1, DMA_CHANNEL4, (uint32_t)data);
    dma_set_number_of_data(DMA1, DMA_CHANNEL4, size);
    dma_set_read_from_memory(DMA1, DMA_CHANNEL4);
    dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL4);
    dma_set_peripheral_size(DMA1, DMA_CHANNEL4, DMA_CCR_PSIZE_8BIT);
    dma_set_memory_size(DMA1, DMA_CHANNEL4, DMA_CCR_MSIZE_8BIT);
    dma_set_priority(DMA1, DMA_CHANNEL4, DMA_CCR_PL_VERY_HIGH);

    dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL4);

    dma_enable_channel(DMA1, DMA_CHANNEL4);

    usart_enable_tx_dma(USART1);
}

volatile int transfered = 0;

void dma1_channel4_isr(void)
{
    printf("接收到dma1_channel4 中断\r\n");
    //接收到
    if ((DMA1_ISR & DMA_ISR_TCIF4) != 0)
    {
        DMA1_IFCR |= DMA_IFCR_CTCIF4;
        printf("dma数据已发送完成\r\n");
        transfered = 1;
    }

    dma_disable_transfer_complete_interrupt(DMA1, DMA_CHANNEL4);

    usart_disable_tx_dma(USART1);

    dma_disable_channel(DMA1, DMA_CHANNEL4);
}

void dma_read(char *data, int size)
{
    /*
     * 串口1 接收使用通道 5
     */

    /* Reset DMA channel*/
    dma_channel_reset(DMA1, DMA_CHANNEL5);

    dma_set_peripheral_address(DMA1, DMA_CHANNEL5, (uint32_t)&USART1_DR);
    dma_set_memory_address(DMA1, DMA_CHANNEL5, (uint32_t)data);
    dma_set_number_of_data(DMA1, DMA_CHANNEL5, size);
    dma_set_read_from_peripheral(DMA1, DMA_CHANNEL5);
    dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL5);
    dma_set_peripheral_size(DMA1, DMA_CHANNEL5, DMA_CCR_PSIZE_8BIT);
    dma_set_memory_size(DMA1, DMA_CHANNEL5, DMA_CCR_MSIZE_8BIT);
    dma_set_priority(DMA1, DMA_CHANNEL5, DMA_CCR_PL_HIGH);

    dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);

    dma_enable_channel(DMA1, DMA_CHANNEL5);

    usart_enable_rx_dma(USART1);
}

volatile int received = 0;

void dma1_channel5_isr(void)
{
    printf("接收到 dma1_channel5 中断\r\n");
    if ((DMA1_ISR & DMA_ISR_TCIF5) != 0)
    {
        DMA1_IFCR |= DMA_IFCR_CTCIF5;
        printf("dma数据已接收完成\r\n");
        received = 1;
    }

    dma_disable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);

    usart_disable_rx_dma(USART1);

    dma_disable_channel(DMA1, DMA_CHANNEL5);
}
// 发送大量数据  在发送的同时 闪烁 led
void test_one(void)
{
    char tx[5000];
    int32_t i = 0;
    for (i = 0; i < 5000; i++)
    {
        tx[i] = 'p';
    }
    int tx_len = 5000;
    printf("要开始发送dma数据了\r\n");
    printf("tx 数组内容长度 %d \r\n", sizeof(tx));

    transfered = 0;
    dma_write(tx, tx_len);
    received = 0;
    while (1)
    {
        gpio_toggle(GPIOA, GPIO8); /* LED on/off */
        delay_ms(1000);
    }

    return 0;
}
//dma 写数据 在 dma write 收到数据后 更改dma 写的数据
void test_two(void)
{
    nvic_enable_irq(NVIC_DMA1_CHANNEL4_IRQ);
    nvic_enable_irq(NVIC_DMA1_CHANNEL5_IRQ);
    // rcc_periph_clock_enable(RCC_AFIO);
    /* Configure the EXTI subsystem. */
    // exti_select_source(EXTI14, NVIC_DMA1_CHANNEL4_IRQ);
    // exti_set_trigger(EXTI14, EXTI_TRIGGER_BOTH);
    // exti_enable_request(EXTI14);

    // exti_select_source(EXTI15, NVIC_DMA1_CHANNEL5_IRQ);
    // exti_set_trigger(EXTI15, EXTI_TRIGGER_BOTH);
    // exti_enable_request(EXTI15);

    char tx[10] = "abcdefg\r\n";
    int tx_len = 10;
    char rx[7] = "fffffff";
    int rx_len = 6;

    transfered = 0;
    dma_write(tx, tx_len);
    received = 0;
    dma_read(rx, rx_len);

    /* Blink the LED (PA8) on the board with every transmitted byte. */
    while (1)
    {
        gpio_toggle(GPIOA, GPIO8); /* LED on/off */
        while (transfered != 1)
        {
            if (received == 1)
            {
                tx[1] = rx[0];
                tx[2] = rx[1];
                tx[3] = rx[2];
                tx[4] = rx[3];
                tx[5] = rx[4];
                tx[6] = rx[5];
                received = 0;
                dma_read(rx, rx_len);
            }
        }
        tx[0]++;
        if (tx[0] > 'z')
            tx[0] = 'a';
        transfered = 0;
        dma_write(tx, tx_len);
    }
}

void test_usart_dma(void)
{
    rcc_periph_clock_enable(RCC_DMA1);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);

    test_one();
    //test_two();

    return 0;
}