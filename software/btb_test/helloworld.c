#include <stdio.h>

int main(void) {
	int i; 

	for(i = 0; i < 500; i++) {
		asm volatile("nop");
	}

	*((volatile unsigned int *) 0x80001000) = 1;
	return 0;
}

