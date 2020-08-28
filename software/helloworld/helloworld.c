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
int main(void) {
	int i;

	uart_puts("hello,world!");
	wait_uart();

	return 0;
}

extern void (* _exit)(int);
void exit(int stat) {
	_exit(stat);
}
