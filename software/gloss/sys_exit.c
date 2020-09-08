void _exit(int status) {

	while(1)
		asm volatile("nop");
}
