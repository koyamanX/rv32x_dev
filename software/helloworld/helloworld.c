void wait_uart(void) {
	while(*((volatile unsigned int *) 0x40000004))
		;
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
int main(void) {
	uart_puts("hello,world!");

	return 0;
}
