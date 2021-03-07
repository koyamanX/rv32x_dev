#ifndef TCM_H
#define TCM_H
declare tcm {
	input addr[32];
	input wdata[32];
	input byteen[3];
	output rdata[32];
	func_in read(addr, byteen);
	func_in write(addr, byteen, wdata);
	func_out ready();
}
#endif
