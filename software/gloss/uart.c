#include "uart.h"
#include <stdlib.h>

int uart_putchar(int ch) {

	while(UART_TX_GET_STAT_FULL())
		asm volatile("nop");

	*UART_TX_BUF = ch;
	UART_TX_SET_EN(1);

	return (unsigned int) ch;
}

int uart_getchar(void) {
	int ch = -1;

	UART_RX_SET_EN(1);

	/* Blocking */
	while(UART_RX_GET_STAT_EMPTY()) {
		asm volatile("nop");
	}
	ch = *UART_RX_BUF;
	return ch;
}

void uart_rx_interrupt_handler(void) {
	exit(1);
}
void uart_tx_interrupt_handler(void) {
	exit(1);
}


