#ifndef CLINT_H
#define CLINT_H
#define CLINT_BASE_ADDR 0x2000000
#define CLINT_MSIP_ADDR_OFFSET 0x0000
#define CLINT_MTIMECMP_ADDR_OFFSET 0x4000
#define CLINT_MTIME_ADDR_OFFSET 0xbff8
#define CLINT_SIZE 0xc000
declare clint {
	input addr[16];
	input wdata[32];
	output rdata[32];
	func_in read(addr);
	func_in write(addr, wdata);
	func_out ready();
	func_out store_access_fault();
	func_out load_access_fault();
	func_out software_interrupt_hart0();
	func_out timer_interrupt_hart0();
}
#endif
