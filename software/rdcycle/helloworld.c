#include <stdio.h>

int main(void) {
	register unsigned int cycle = 0;

	asm volatile("rdcycle %0" :: "r" (cycle));
	printf("cycle : %d\n", cycle);
	asm volatile("rdcycle %0" :: "r" (cycle));
	printf("cycle : %d\n", cycle);
	asm volatile("rdcycle %0" :: "r" (cycle));
	printf("cycle : %d\n", cycle);
	asm volatile("rdcycle %0" :: "r" (cycle));
	printf("cycle : %d\n", cycle);
	asm volatile("rdcycle %0" :: "r" (cycle));
	printf("cycle : %d\n", cycle);

	while(1) {
		asm volatile("rdcycle %0" :: "r" (cycle));
		if(cycle >= 10000000)
			break;
	}
	printf("cycle : %d\n", cycle);

	return 0;
}

