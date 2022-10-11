#ifndef TCM_H
#define TCM_H
#define IMEM 2'b01
#define DMEM 2'b10
declare tcm
{
	param_str filename;
	input addr[32];
	input wdata[32];
	input byteen[3];
	input type[2];
	output rdata[32];
	func_in reset();
	func_in read(addr, byteen, type);
	func_in write(addr, byteen, wdata);
	func_out iready();
	func_out dready();
}
#endif
