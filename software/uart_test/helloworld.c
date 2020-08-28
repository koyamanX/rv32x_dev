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
	*((volatile unsigned int *) 0x40000014) = 0x1;
	char ch = 0;
	int i = 0;

	while(1) {
		if(((*((volatile unsigned int *) 0x40000014)) & 0x4) == 0) {
			ch = *((volatile unsigned int *) 0x40000010);
			uart_putchar(ch);
			i++;
		}
		if(i > 10) {
			uart_putchar('\n');
			break;
		}
	}
	
	uart_puts("hello,world");
	wait_uart();

	return 0;
}

extern void (* _exit)(int);
void exit(int stat) {
	_exit(stat);
}
