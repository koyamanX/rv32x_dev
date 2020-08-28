void wait_uart(void) {
	while(*((volatile unsigned int *) 0x40000004) & 0x2)
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
static inline void mstatus_w(unsigned int x) {
	asm volatile("csrw mstatus, %0" : : "r" (x));
}
static inline void mie_w(unsigned int x) {
	asm volatile("csrw mie, %0" : : "r" (x));
}
volatile int x = 0; 
int main(void) {
	*((volatile unsigned int *) 0x20004000) = 0xffff;
	mie_w(0x80);
	mstatus_w(0x8);

	while(1) {
		if(x > 3) {
			mie_w(0x00);
			uart_puts("hello");
			return 0;
		}
	}
	return 1;
}
typedef union {
	struct {
		unsigned int lo;
		unsigned int hi;
	} parcel;
	unsigned long long val;
} timer_t;

__attribute__ ((interrupt ("machine"))) void timer_handler(void) {
	timer_t mtime;
	timer_t mtimecmp;

	mtime.parcel.hi = *((volatile unsigned int *)0x20008ffc);
	mtime.parcel.lo = *((volatile unsigned int *)0x20008ff8);
	mtimecmp.parcel.hi = *((volatile unsigned int *)0x20004004);
	mtimecmp.parcel.lo = *((volatile unsigned int *)0x20004000);
	mtimecmp.val = mtime.val + 0xffff;
	*((volatile unsigned int *)0x20004004) = mtimecmp.parcel.hi;
	*((volatile unsigned int *)0x20004000) = mtimecmp.parcel.lo;

	x++;

	return ;
}
