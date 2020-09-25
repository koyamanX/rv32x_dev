#include <stdio.h>

int main(void) {
	register unsigned long long int cycle = 0;
	unsigned int data[10];
	int i;

	for(i = 0; i < 10; i++) {
		asm volatile("rdcycle %0" :: "r" (cycle));
		data[i] = cycle;
	}

	for(i = 0; i < 10; i++) {
		printf("cycle : %d\n", data[i]);
	}

	return 0;
}

