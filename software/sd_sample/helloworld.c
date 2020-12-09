#include <stdio.h>

#define SD_BASE 0x10001000
#define SD_STATUS ((volatile unsigned int *) (SD_BASE + 0x200))
#define SD_ADRS ((volatile unsigned int *) (SD_BASE + 0x214))
#define SD_OP ((volatile unsigned int *) (SD_BASE + 0x210))
#define SD_DATA_BASE ((volatile unsigned int *) (SD_BASE + 0x0))

int main(void) {
	int i;
	unsigned int buf[512];

	if((*SD_STATUS) == 0x0) {
		printf("waiting for init\n");
	}
	while((*SD_STATUS) != 0x3) {
		asm volatile("nop");
	}
	if((*SD_STATUS) == 0x3) {
		printf("inited & idle\n");
	}
	printf("read block address:0x%08x\n", 0x00000000);
	*SD_ADRS = 0x00000000;
	*SD_OP = 0x00000001;

	while((*SD_OP != 0x0) || (*SD_STATUS != 0x3)) {
		asm volatile("nop");
	}
	
	for(int i = 0; i < 128; i++)
		printf("%08x:%08x\n", i*4, ((volatile unsigned int *)SD_DATA_BASE)[127-i]);

	return 0;
}

