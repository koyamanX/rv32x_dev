#ifndef IFETCH_UNIT_H
#define IFETCH_UNIT_H

#define CLEAN 1'b0
#define DIRTY 1'b1
#define VALID 1'b1
#define INVALID 1'b0

declare ifetch_unit
{
	func_in reset();
	/* CPU <-> Cache Unit */
	input addr[32];
	output insts[128];
	/* Always word access */
	/* if need to cancel operation, then deassert these signals before valid
		however contents of cache will be updated even with cancelation */
	func_in read(addr); /* must be asserted until valid */
	func_out ready;		/* hit */

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
	output vaddr[32];
	func_out walk(satp, vaddr);
	input pte[32];
	input pte_levels[2];
	func_in pte_ready();
	func_in walk_page_fault();
	func_in walk_access_fault();

	input priv_mode[2];

	/* Cache Unit <-> Memory Interface */
	output mem_addr[32];
	input mem_rdata[128];
	func_out mem_read(mem_addr);
	func_in mem_ready;

	output pma_addr[32];
	func_out pma_check(pma_addr);
	func_in pma_executable();
	func_in pma_readable();
	func_in pma_word();
	func_in pma_halfword();
	func_in pma_cacheable();
	func_in pma_empty();

#ifdef CV
	output sseg_l12[12]; // execute stage pc low 12bit
	func_out debug_sseg_l12(sseg_l12);
	output sseg_h12[12]; // execute stage next pc low 12bit
	func_out debug_sseg_h12(sseg_h12);
	func_out debug_led_1();
	func_out debug_led_2();
	func_out debug_led_3();
	func_out debug_led_4();
	func_out debug_led_5();
	func_out debug_led_6();
	func_out debug_led_7();
	func_out debug_led_8();
	func_out debug_led_9();
	func_out debug_led_10();
#endif
}
#endif
