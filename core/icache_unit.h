#ifndef ICACHE_UNIT_H
#define ICACHE_UNIT_H

#define CLEAN 1'b0
#define DIRTY 1'b1
#define VALID 1'b1
#define INVALID 1'b0

/* If hits, rdata and valid signals available with no delay */
declare icache_unit {
	func_in reset();
	/* CPU <-> Cache Unit */
	input adrs[32];
	output rdata[32];
	/* Always word access */
	/* if need to cancel operation, then deassert these signals before valid 
		however contents of cache will be updated even with cancelation */
	func_in read(adrs);				/* must be asserted until valid */
	func_out valid;	/* hit */

	func_in invalidate;
	func_out invalidate_done;

	/* Cache Unit <-> Memory Interface */
	output mem_adrs[32];
	input mem_rdata[128];
	func_out mem_read(mem_adrs);
	func_in mem_valid;
	func_in mem_error;	/* just invalidate the line */
}

#endif
