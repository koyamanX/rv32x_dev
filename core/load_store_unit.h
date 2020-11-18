#ifndef LOAD_STORE_UNIT_H
#define LOAD_STORE_UNIT_H

#define CLEAN 1'b0
#define DIRTY 1'b1
#define VALID 1'b1
#define INVALID 1'b0

#define CACHE_WRITE	1'b1
#define CACHE_READ	1'b0

declare load_store_unit {
	/* CPU <-> Cache Unit */
	input adrs[32];
	output load_data[32];
	input store_data[32];
	input byteen[3];
	func_in reset();
	/* Operations */
	func_in load(adrs, byteen);					/* must be asserted until valid arrives */
	func_in store(adrs, byteen, store_data);	/* must be asserted until valid arrives */
	func_out valid;								/* operation is done */

	func_in cache_flush;
	func_out cache_flush_done;

	input satp_mode;
	input satp_asid[9];
	input satp_ppn[22];
	input mstatus_mxr;
	input priv_mode[2];
	input sstatus_sum;

	/* flush all TLB entries, no writeback */
	func_in tlb_flush();
	func_out tlb_flush_done();

	func_out store_amo_page_fault();
	func_out store_amo_access_fault();
	func_out load_page_fault();
	func_out load_access_fault();

	/* Cache Unit <-> uTLB interfaces */
	input utlb_adrs[32];
	func_in read_utlb(utlb_adrs);
	output utlb_pte[32];
	output utlb_pte_levels[2];
	func_out utlb_pte_valid();
	func_out utlb_page_fault();
	func_out utlb_access_fault();

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
#ifdef DEBUG
	output tohost[32];
	func_out sim_done(tohost);
#endif
}
#endif
