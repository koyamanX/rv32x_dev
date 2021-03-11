#ifndef CACHE_H
#define CACHE_H

#define CLEAN 1'b0
#define DIRTY 1'b1
#define VALID 1'b1
#define INVALID 1'b0

#define CACHE_WRITE	1'b1
#define CACHE_READ	1'b0

declare cache {
	input addr[32];
	output rdata[32];
	input wdata[32];
	input byteen[3];
	func_in reset();
	func_in read(addr, byteen);
	func_in write(addr, byteen, wdata);
	func_out valid;

	func_in cache_flush;
	func_out cache_flush_done;

	func_out store_amo_access_fault();
	func_out load_access_fault();

	/* Cache Unit <-> Memory Interface */
	output pma_addr[32];
	func_out pma_check(pma_addr);
	func_in pma_empty();
	func_in pma_cacheable();

	/* use 32 bit physical address even with sv32 enabled */
	output mem_addr[32];
	input mem_rdata[128];
	output mem_wdata[128];
	output mem_size[3];
	func_out mem_read(mem_addr, mem_size);
	func_out mem_write(mem_addr, mem_size, mem_wdata);
	func_in mem_valid;
}
#endif
