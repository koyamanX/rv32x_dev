#define PLIC_BASE_ADDR ((volatile unsigned int *) 0x0c000000)
#define PLIC_PRIORITY_BASE_ADDR ((volatile unsigned int *) 0x0c000000)
#define PLIC_PRIORITY_BASE_ADDR_10 ((volatile unsigned int *) 0x0c000028)
#define PLIC_PENDING_BASE_ADDR ((volatile unsigned int *) 0x0c001000)
#define PLIC_ENABLE_BASE_ADDR ((volatile unsigned int *) 0x0c002000)
#define PLIC_THRESHOLD_BASE_ADDR ((volatile unsigned int *) 0x0c200000)
#define PLIC_CLAIM_COMPLETE_BASE_ADDR ((volatile unsigned int *) 0x0c200004)

#define UART_TX_BUF ((volatile unsigned int *) 0x40000000)
#define UART_TX_STAT ((volatile unsigned int *) 0x40000004)

#define UART_RX_BUF ((volatile unsigned int *) 0x40000010)
#define UART_RX_STAT ((volatile unsigned int *) 0x40000014)
#define UART_RX_THRESHOLD ((volatile unsigned int *) 0x40000018)

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
void meip_handler(void) {
	int claim;
	
	claim = *PLIC_CLAIM_COMPLETE_BASE_ADDR;
	while(!UART_RX_GET_STAT_EMPTY()) {
		uart_putchar(uart_getchar());
	}
	*PLIC_CLAIM_COMPLETE_BASE_ADDR = claim;
}
int main(int argc, char **argv, char **env) {
	*(PLIC_PRIORITY_BASE_ADDR + 10) = 0x1;
	*PLIC_ENABLE_BASE_ADDR = 0x1 << 10;
	*UART_RX_THRESHOLD = 0x1;
	UART_RX_SET_EN(1);

	while(1) {
		asm volatile("nop");
	}

	return 0;
}

