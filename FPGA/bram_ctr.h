#ifndef BRAM_H
#define BRAM_H
declare bram_ctr
{
	input iaddr[32];
	input daddr[32];
	input wdata[32];
	input ibyteen[3];
	input dbyteen[3];
	output irdata[32];
	output drdata[32];
	func_in reset();
	func_in iread(iaddr, ibyteen);
	func_in dread(daddr, dbyteen);
	func_in dwrite(daddr, dbyteen, wdata);
	func_out iready();
	func_out dready();
}
#endif
