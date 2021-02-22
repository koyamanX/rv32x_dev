#ifndef CLINT_H
#define CLINT_H
declare clint {
	input addr[16];
	input wdata[32];
	output rdata[32];
	func_in read(addr) : rdata;
	func_in write(addr, wdata);
	func_out ready();

	func_out software_interrupt();
	func_out timer_interrupt();
	func_out store_access_fault();
	func_out load_access_fault();
}

#define CLINT_BASE_ADDR 			0x0200_0000
#define CLINT_MSIP_ADDR_OFFSET 		0x0000
#define CLINT_MSIP_SIZE				0x0004
#define CLINT_MTIMECMP_ADDR_OFFSET	0x4000
#define CLINT_MTIMECMP_SIZE			0x0008
#define CLINT_MTIME_ADDR_OFFSET		0x8ff8
#define CLINT_MTIME_SIZE			0x0008
#define CLINT_SIZE					0x0000_c000

#endif
