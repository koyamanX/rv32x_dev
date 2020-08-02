#ifndef BOOTROM_H
#define BOOTROM_H
declare bootrom {
	input port1_radrs[10];
	output port1_rdata[32];
	func_in port1_read(port1_radrs);
	func_out port1_valid();
	func_in reset();
}
#endif
