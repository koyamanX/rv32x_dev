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

	input satp_mode;
	input satp_asid[9];
	input satp_ppn[22];

	func_in tlb_flush();
	func_out tlb_flush_done();

	func_out instruction_access_fault();
	func_out instruction_page_fault();

	output satp[32];
	output vadrs[32];
	output pte[32];
	func_out walk(satp, vadrs);
	func_in pte_valid();

	/* Cache Unit <-> Memory Interface */
	output mem_adrs[32];
	input mem_rdata[128];
	func_out mem_read(mem_adrs);
	func_in mem_valid;
	func_in mem_error;	/* just invalidate the line */
}

#endif
