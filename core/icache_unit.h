#ifndef ICACHE_UNIT_H
#define ICACHE_UNIT_H

#define CLEAN 1'b0
#define DIRTY 1'b1
#define VALID 1'b1
#define INVALID 1'b0

declare icache_unit {
	func_in reset();
	/* CPU <-> Cache Unit */
	input adrs[32];
	output rdata[32];
	/* Always word access */
	/* if need to cancel operation, then deassert these signals before valid 
		however contents of cache will be updated even with cancelation */
	func_in read(adrs);				/* must be asserted until valid */
	func_out valid;					/* hit */

	/* invalidate all cache entries */
	func_in invalidate;
	func_out invalidate_done;

	/* satp from CPU core for instruction page access */
	input satp_mode;
	input satp_asid[9];
	input satp_ppn[22];

	/* flush all TLB entries, no writeback */
	func_in tlb_flush();
	func_out tlb_flush_done();

	/* instruction faults, no PMA check on instruction fetch
		however, pma is checked on page_table_walk, and instruction_access_fault
		is raised if page_table_walk violates PMAs
	*/
	func_out instruction_access_fault();
	func_out instruction_page_fault();

	/* Cache Unit <-> Page table walker Interface */
	/* satp for page table walk to page table walker */
	output satp[32];
	output vadrs[32];
	func_out walk(satp, vadrs);
	input pte[32];
	func_in pte_valid();
	func_in page_fault();
	func_in access_fault();

	/* Cache Unit <-> Memory Interface */
	output mem_adrs[32];
	input mem_rdata[128];
	func_out mem_read(mem_adrs);
	func_in mem_valid;
}
#endif
