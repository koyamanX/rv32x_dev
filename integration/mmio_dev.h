#ifndef MMIO_DEV_H
#define MMIO_DEV_H
declare mmio_dev {
	input adrs[16];
	output rdata[32];
	input wdata[32];
	func_in read(adrs) : rdata;
	func_in write(adrs, wdata);
	func_out valid();

	func_in reset();

	output TXD;
	input RXD;
}
#endif
