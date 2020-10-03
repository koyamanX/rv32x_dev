void wait_uart(void) {
	while((*((volatile unsigned int *) 0x40000004) & 0x6) != 0x4)
		;
}
void uart_putchar(int ch) {
	while((*((volatile unsigned int *) 0x40000004)) & 0x8)
		;
	*((volatile unsigned int *) 0x40000000) = ch;
	if(!(*((volatile unsigned int *)0x40000004) & 0x1))
		*((volatile unsigned int *) 0x40000004) = 1;
}
void uart_puts(char *str) {
	while(*str) {
		uart_putchar(*str);
		str++;
	}
	uart_putchar('\n');
}

static inline void sie_w(unsigned int x) {
	asm volatile("csrw sie, %0" : : "r" (x));
}
volatile int x = 0; 
int main(void) {
	*((volatile unsigned int *) 0x20000000) = 0x1;

	while(1) {
		if(x > 0) {
			uart_puts("hello,world!");
			wait_uart();
			return 0;
		}
	}
	return 1;
}
__attribute__ ((interrupt ("supervisor"))) void software_handler(void) {
	*((volatile unsigned int *) 0x20000000) = 0x0;
	x++;

	return ;
}
