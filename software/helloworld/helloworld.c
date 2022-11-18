#include <stdio.h>

int main(void)
{
	printf("hello,world!\n");
	*((unsigned int *)(0x80001000)) = 0x00000001;
	return 0;
}
