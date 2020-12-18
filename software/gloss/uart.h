#ifndef UART_H
#define UART_TX_BUF ((volatile unsigned int *) 0x40000000)
#define UART_TX_STAT ((volatile unsigned int *) 0x40000004)

#define UART_RX_BUF ((volatile unsigned int *) 0x40000010)
#define UART_RX_STAT ((volatile unsigned int *) 0x40000014)

#define UART_TX_GET_STAT_FULL() (((*(UART_TX_STAT)) & 0x00000008) >> 3)
#define UART_TX_GET_STAT_EMPTY() (((*(UART_TX_STAT)) & 0x00000004) >> 2)
#define UART_TX_GET_STAT_BUSY() (((*(UART_TX_STAT)) & 0x00000002) >> 1)
#define UART_TX_GET_STAT_EN() (((*(UART_TX_STAT)) & 0x00000001))
#define UART_TX_SET_EN(en) (*(UART_TX_STAT) = *(UART_TX_STAT) | ((en) & 0x1));

#define UART_RX_GET_STAT_FULL() (((*(UART_RX_STAT)) & 0x00000008) >> 3)
#define UART_RX_GET_STAT_EMPTY() (((*(UART_RX_STAT)) & 0x00000004) >> 2)
#define UART_RX_GET_STAT_BUSY() (((*(UART_RX_STAT)) & 0x00000002) >> 1)
#define UART_RX_GET_STAT_EN() (((*(UART_RX_STAT)) & 0x00000001))
#define UART_RX_SET_EN(en) (*(UART_RX_STAT) = *(UART_RX_STAT) | ((en) & 0x1));

typedef union {
	struct {
		unsigned int en : 1;	
		unsigned int busy : 1;	
		unsigned int empty : 1;
		unsigned int full : 1;
		unsigned int unused: 28;
	} stat;
	unsigned int val;
} uart_tx_stat_t;

typedef uart_tx_stat_t uart_rx_stat_t;

int uart_putchar(int ch);
int uart_getchar(void);

extern __attribute__((weak)) void uart_rx_interrupt_handler(void);
extern __attribute__((weak)) void uart_tx_interrupt_handler(void);
#endif
