#include "uart.h"

int uart_putchar(int ch) {

	while(UART_TX_GET_STAT_FULL())
		asm volatile("nop");

	*UART_TX_BUF = ch;
	UART_SET_EN(1);

	while(UART_TX_GET_STAT_BUSY())
		asm volatile("nop");

	return (unsigned int) ch;
}


