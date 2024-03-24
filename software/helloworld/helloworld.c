#include <stdio.h>
#define UART_TX_STAT ((volatile unsigned int *)0x40000004)

int main(void)
{
	printf("hello,world!\n");
	while (*UART_TX_STAT != 0x05) // prevent simulator exit while sending characters
		;
	return 0;
}
