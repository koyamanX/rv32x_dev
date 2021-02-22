#ifndef BOOTROM_H
#define BOOTROM_H

declare bootrom {
	input addr[12];
	output rdata[32];
	func_in read(addr);
	func_out ready();
	func_in reset();
}

#endif
