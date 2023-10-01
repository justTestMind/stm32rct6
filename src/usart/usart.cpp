
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
//#include <unistd.h>

void usart_setup(void)
{
	/* Setup GPIO pin GPIO_USART1_TX. */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
				  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
				  GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART1, 9600);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	//rcc_periph_clock_enable(RCC_AFIO);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART1);
}
//用于c语言的串口打印
int _write(int file, char *ptr, int len)
{
	int i;

	if (file == 1)
	{
		for (i = 0; i < len; i++)
			usart_send_blocking(USART1, ptr[i]);
		return i;
	}

	errno = EIO;
	return -1;
}

 void uart1_puts(const char *str){
	
	const char* p = NULL;
	for(p = str;*p;p++){
		usart_send_blocking(USART1, *p);
	}
 }


int uart_print(char * buf, const char *fmt, ...)
{
	va_list args;
	int i;
	va_start(args, fmt);
	i=vsprintf(buf,fmt,args);
	va_end(args);
	uart1_puts(buf);
	return i;
}
//用于
int uart_println(char * buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=vsprintf(buf,fmt,args);
	va_end(args);
	buf = strcat(buf,"\r\n");
	uart1_puts(buf);

	return i+2;
}

// int fputc(int ch, FILE *f)
// {
//       //HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1,HAL_MAX_DELAY);
// 	  usart_send_blocking(USART1, ch);
//       return  EOF;
// }


// void test_usart(void)
// {
// 	uint8_t tempetature, humidity;
// 	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);

// 	while (1)
// 	{

// 		printf("hello world \r\n");
// 		// TestDHT11();
// 		delay_ms(1000);
// 		char t = usart_recv_blocking(USART1);
// 		// if (t == 49)
// 		// {
// 		// 	gpio_clear(GPIOA, GPIO8);
// 		// }
// 		// else if (t == 50)
// 		// {
// 		// 	gpio_set(GPIOA, GPIO8);
// 		// }

// 		switch (t)
// 		{
// 		case '1':
// 			gpio_set(GPIOA, GPIO8);
// 			break;
// 		case '2':
// 			gpio_clear(GPIOA, GPIO8);
// 			break;
// 		default:
// 			break;
// 		}

// 		printf("rct6 收到指令 %x \r\n", t);
// 	}
// }



