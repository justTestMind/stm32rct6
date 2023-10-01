#ifndef __USART_H
#define __USART_H

#include <stdio.h>

//抽离usart 信息

void usart_setup(void);

int uart_print(char * buf, const char *fmt, ...);
int uart_println(char * buf, const char *fmt, ...);

#endif