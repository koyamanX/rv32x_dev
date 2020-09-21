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

__attribute__ ((interrupt ("machine"))) void machine_software_interrupt_handler(void) {
	return ;
}
__attribute__ ((interrupt ("machine"))) void machine_timer_interrupt_handler(void) {
	return ;
}
__attribute__ ((interrupt ("machine"))) void machine_external_interrupt_handler(void) {
	return ;
}
