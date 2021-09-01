
#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <stdio.h>
#include <errno.h>
#include <libopencm3/stm32/timer.h>
#include "./delay.h"
#include "./dht11.h"


static void usart_setup(void)
{
	/* Setup GPIO pin GPIO_USART1_TX. */
     rcc_periph_clock_enable(RCC_GPIOA);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART1, 9600);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART1);
    
	
}
int _write(int file, char *ptr, int len)
{
	int i;

	if (file == 1) {
		for (i = 0; i < len; i++)
			usart_send_blocking(USART1, ptr[i]);
		return i;
	}

	errno = EIO;
	return -1;
}

int main(void){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);
    
    rcc_periph_clock_enable(RCC_USART1);

    gpio_set_mode(GPIOA,GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO3);
	gpio_set_mode(GPIOC,GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO2);
    usart_setup();
    uint8_t tempetature,humidity;
    DHT11_Init();
	
    while (1)
    {
        DHT11_Read_Data(&tempetature,&humidity);
        printf("温度： %d \r\n 湿度: %d  \r\n",tempetature,humidity);
		//gpio_set(GPIOA,GPIO3);
        //delay_ms(3000);
		//gpio_clear(GPIOA,GPIO3);
        delay_ms(3000);
		gpio_toggle(GPIOC,GPIO2);
		

        //gpio_toggle(GPIOA,GPIO3);

    }

}