
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
