void _exit(int exit_status) {

	while(1)
		asm volatile("nop");
}
