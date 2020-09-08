#define INSTRUCTION_ADDRESS_MISALIGNED 0
#define INSTRUCTION_ACCESS_FAULT        1
#define ILLEGAL_INSTRUCTION             2
#define BREAKPOINT                      3
#define LOAD_ADDRESS_MISALIGNED         4
#define LOAD_ACCESS_FAULT               5
#define STORE_AMO_ADDRESS_MISALIGNED    6
#define STORE_AMO_ACCESS_FAULT          7
#define ENVIRONMENT_CALL_FROM_U_MODE    8
#define ENVIRONMENT_CALL_FROM_S_MODE    9
#define ENVIRONMENT_CALL_FROM_M_MODE    11
#define INSTRUCTION_PAGE_FAULT          12
#define LOAD_PAGE_FAULT                 13
#define STORE_AMO_PAGE_FAULT            15

extern int syscall(int, int, int, int, int, int, int, int);
extern void exit(int);

__attribute__ ((interrupt ("machine"))) void exception_handler(void) {
	register unsigned int a0 asm("a0");
	register unsigned int a1 asm("a1");
	register unsigned int a2 asm("a2");
	register unsigned int a3 asm("a3");
	register unsigned int a4 asm("a4");
	register unsigned int a5 asm("a5");
	register unsigned int a6 asm("a6");
	register unsigned int a7 asm("a7");
	register unsigned int mepc;
	register unsigned int mcause;

	asm volatile("csrr %0, mcause" : "=r"(mcause):);

	switch(mcause) {
		case ENVIRONMENT_CALL_FROM_U_MODE:
		case ENVIRONMENT_CALL_FROM_M_MODE:
			syscall(a7, a0, a1, a2, a3, a4, a5, a6);
			break;
		default:
			exit(mcause);
			break;
	}

	asm volatile("csrr %0, mepc" : "=r"(mepc):);
	mepc += 4;
	asm volatile("csrw mepc, %0" :: "r"(mepc));	

	return ;
}
__attribute__ ((interrupt ("machine"))) void trap_handler(void) {
	return ;
}
__attribute__ ((interrupt ("machine"))) void machine_software_interrupt_handler(void) {
	return ;
}
__attribute__ ((interrupt ("machine"))) void machine_timer_interrupt_handler(void) {
	return ;
}
__attribute__ ((interrupt ("machine"))) void machine_external_interrupt_handler(void) {
	return ;
}
