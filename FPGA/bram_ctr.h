#ifndef BRAM_H
#define BRAM_H
declare bram_ctr
{
	input addr[32];
	input wdata[32];
	input byteen[3];
	input type[2];
	output rdata[32];
	func_in read(addr, byteen, type);
	func_in write(addr, byteen, wdata);
	func_out iready();
	func_out dready();
}

#define IMEM 2'b01
#define DMEM 2'b10
#endif
