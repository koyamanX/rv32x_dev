#ifndef CACHE_H
#define CACHE_H

#define CLEAN 1'b0
#define DIRTY 1'b1
#define VALID 1'b1
#define INVALID 1'b0

#define CACHE_WRITE	1'b1
#define CACHE_READ	1'b0

declare cache {
	/* CPU <-> Cache Unit */
	input adrs[32];
	output rdata[32];
	input wdata[32];
	input byteen[3];
	func_in reset();
	func_in read(adrs, byteen);				/* must be asserted until valid arrives */
	func_in write(adrs, byteen, wdata);		/* must be asserted until valid arrives */
	func_out valid;							/* hit */

	func_in cache_flush;
	func_out cache_flush_done;

	func_out store_amo_access_fault();
	func_out load_access_fault();

	/* Cache Unit <-> Memory Interface */
	output pma_adrs[32];
	func_out pma_check(pma_adrs);
	func_in pma_empty();
	func_in pma_cacheable();

	/* use 32 bit physical address even with sv32 enabled */
	output mem_adrs[32];
	input mem_rdata[128];
	output mem_wdata[128];
	output mem_size[3];
	func_out mem_read(mem_adrs, mem_size);
	func_out mem_write(mem_adrs, mem_size, mem_wdata);
	func_in mem_valid;
}
#endif
