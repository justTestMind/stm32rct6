
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <errno.h>
#include "./blue_tooth.h"
#include <libopencm3/stm32/exti.h>
#include <libopencm3/cm3/nvic.h>

void test_blur_tooth(void){
	//运行代码等待蓝牙做出响应
	rcc_periph_clock_enable(RCC_AFIO);
	nvic_enable_irq(NVIC_USART2_IRQ);
	//nvic_enable_irq(NVIC_EXTI0_IRQ);
	buletooth_usart_setup();

	while (1)
	{

		printf("hello world \r\n");
		// TestDHT11();
		gpio_set(GPIOA,GPIO6);
		delay_ms(1000);
		char t = usart_recv_blocking(USART1);
		// if (t == 49)
		// {
		// 	gpio_clear(GPIOA, GPIO8);
		// }
		// else if (t == 50)
		// {
		// 	gpio_set(GPIOA, GPIO8);
		// }

		usart_send_blocking(USART2, "A");
		usart_send_blocking(USART2, "T");
		usart_send_blocking(USART2, '\r');
		usart_send_blocking(USART2, '\n');
	delay_ms(50);
		// 	for(t=0;t<10;t++) 			//最长等待50ms,来接收BT05模块的回应
		// {
		// 	if((USART_SR(USART2) & USART_SR_RXNE) == 0){
		// 		break;
		// 	}
			
		// 	delay_ms(5);
		// }

		// gpio_set(GPIOA,GPIO6);
		// delay_ms(1000);

	}


}

void buletooth_usart_setup(void)
{
	/* Setup GPIO pin GPIO_USART2_TX. */
	// GPIOA 2 TX  GPIOA 3 RX
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART2);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
				  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
				  GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART2, 9600);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX_RX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	USART_CR1(USART2) |= USART_CR1_RXNEIE;
	

	/* Finally enable the USART. */
	usart_enable(USART2);

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
				  GPIO_CNF_OUTPUT_PUSHPULL, GPIO6);
	
}


void usart2_isr(void)
{
	static char data[16];
	static int myIndex = 0;

	/* Check if we were called because of RXNE. */
	if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0) &&
	    ((USART_SR(USART2) & USART_SR_RXNE) != 0)) {
		/* Indicate that we got data. */
	
		/* Retrieve the data from the peripheral. */
		data[myIndex] = usart_recv(USART2);
		myIndex++;
		myIndex = myIndex % 16;
		//printf("串口2  接收非空 \r\n");

		//printf("串 %d \r\n", data);

		/* Enable transmit interrupt so it sends back the data. */
		USART_CR1(USART2) |= USART_CR1_TXEIE;
	}

	/* Check if we were called because of TXE. */
	if (((USART_CR1(USART2) & USART_CR1_TXEIE) != 0) &&
	    ((USART_SR(USART2) & USART_SR_TXE) != 0)) {
		USART_CR1(USART2) &= ~USART_CR1_TXEIE;
		/* Indicate that we are sending out data. */
		//printf("串口2  发送为空 \r\n");

		/* Put data into the transmit register. */
		// 回显串口2收到的数据
		// printf("myIndex %d \r\n", myIndex);
		// if(myIndex==2){
		printf("u21 %d \r\n", data[0]);
		printf("u22 %d \r\n", data[1]);
		//}
		/* Disable the TXE interrupt as we don't need it anymore. */
		
	}
}
