void wait_uart(void) {
	while(*((volatile unsigned int *) 0x40000004) & 0x2)
		asm volatile("nop");
}
void uart_putchar(int ch) {
	wait_uart();
	*((volatile unsigned int *) 0x40000000) = ch;
	*((volatile unsigned int *) 0x40000004) = 1;
	wait_uart();
}
void uart_puts(char *str) {
	while(*str) {
		uart_putchar(*str);
		str++;
	}
	uart_putchar('\n');
}
static inline void mstatus_w(unsigned int x) {
	asm volatile("csrw mstatus, %0" : : "r" (x));
}
static inline void mie_w(unsigned int x) {
	asm volatile("csrw mie, %0" : : "r" (x));
}
volatile int x = 0; 
int main(void) {
	*((volatile unsigned int *) 0x20000000) = 0x1;
	mie_w(0x8);
	mstatus_w(0x8);

	while(1) {
		if(x > 0) {
			mie_w(0x00);
			uart_puts("hello");
			wait_uart();
			return 0;
		}
	}
	return 1;
}
__attribute__ ((interrupt ("machine"))) void software_handler(void) {
	x++;

	return ;
}
