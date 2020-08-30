#define PLIC_PRIORITY_REGISTER1 0x0c000004
#define PLIC_PRIORITY_REGISTER2 0x0c000008
#define PLIC_PRIORITY_REGISTER3 0x0c00000c
#define PLIC_ENABLE_REGISTER1   0x0c002000
#define PLIC_THRESHOLD_REGISTER 0x0c200000
#define PLIC_CLAIM_REGISTER		0x0c200004
#define UART_RX_BUF 0x40000010
#define UART_RX_STATUS 0x40000014
#define NULL ((void *) 0x00000000)

char buf[1024];
int i = 0;
volatile int interrupt_cnt = 0;
extern void (* _exit)(int);
void exit(int stat) {
	_exit(stat);
}

static inline void mstatus_w(unsigned int x) {
	asm volatile("csrw mstatus, %0" : : "r" (x));
}
static inline void mie_w(unsigned int x) {
	asm volatile("csrw mie, %0" : : "r" (x));
}
void wait_uart(void) {
	while((*((volatile unsigned int *) 0x40000004) & 0x6) != 0x4)
		;
}
void uart_putchar(int ch) {
	while((*((volatile unsigned int *) 0x40000004)) & 0x8)
		;
	*((volatile unsigned int *) 0x40000000) = ch;
	*((volatile unsigned int *) 0x40000004) = 1;
}
void uart_puts(char *str) {
	while(*str) {
		uart_putchar(*str);
		str++;
	}
	uart_putchar('\n');
}


void uart_tx_handler(void) {
	exit(1);
}
void uart_rx_handler(void) {
	buf[i] = *((volatile unsigned int *)UART_RX_BUF);
	i++;

	if(interrupt_cnt > 3) {
		*((volatile unsigned int *)PLIC_ENABLE_REGISTER1) = 0x0;
	}
	interrupt_cnt++;
}
void (* plic_interrupt_handler[])(void) = {
	NULL,
	uart_rx_handler,
	uart_tx_handler,
};

__attribute__ ((interrupt ("machine"))) void external_isr(void) {
	unsigned int claim;
	claim = *((volatile unsigned int *)PLIC_CLAIM_REGISTER);
	
	if((claim == 1) || (claim == 2)) {
		plic_interrupt_handler[claim]();	
	} else {
		exit(1);
	}
	
	*((volatile unsigned int *)PLIC_CLAIM_REGISTER) = claim;
	return ;
}
int main(void) {

	*((volatile unsigned int *) 0x40000014) = 0x1;
	*((volatile unsigned int *) 0x40000018) = 0x3;
	*((volatile unsigned int *)PLIC_PRIORITY_REGISTER1) = 0x7;
	*((volatile unsigned int *)PLIC_PRIORITY_REGISTER2) = 0x7;
	*((volatile unsigned int *)PLIC_PRIORITY_REGISTER3) = 0x7;
	*((volatile unsigned int *)PLIC_ENABLE_REGISTER1) = 0x2;
	*((volatile unsigned int *)PLIC_THRESHOLD_REGISTER) = 0x0;

	uart_puts("enter some character");
	wait_uart();
	mie_w(0x800);
	mstatus_w(0x8);

	while(1) {
		if(interrupt_cnt > 3)
			break;
	}

	buf[i] = '\0';
	uart_puts(buf);
	uart_puts("hello");
	wait_uart();

	return 0;
}
