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
#ifdef CV
	output sseg_l12[12]; // execute stage pc low 12bit
	func_out debug_sseg_l12(sseg_l12);
	output sseg_h12[12]; // execute stage next pc low 12bit
	func_out debug_sseg_h12(sseg_h12);
#endif
}

#define IMEM 2'b01
#define DMEM 2'b10
#endif
