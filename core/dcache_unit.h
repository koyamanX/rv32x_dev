#ifndef DCACHE_UNIT_H
#define DCACHE_UNIT_H
#include "opcode.h"

#define CLEAN 1'b0
#define DIRTY 1'b1
#define VALID 1'b1
#define INVALID 1'b0

#define CACHE_WRITE	1'b1
#define CACHE_READ	1'b0

declare dcache_unit {
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

	/* Cache Unit <-> Memory Interface */
	output mem_adrs[32];
	input mem_rdata[128];
	output mem_wdata[128];
	output mem_size[3];
	func_out mem_read(mem_adrs, mem_size);
	func_out mem_write(mem_adrs, mem_size, mem_wdata);
	func_in mem_valid;
}

#endif
