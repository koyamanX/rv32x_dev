#ifndef PTW_H
#define PTW_H
declare ptw {
	func_in reset();
	input satp[32];
	input vadrs[32];
	func_in walk(satp, vadrs);
	func_out page_fault();
	output pte[32];
	output pte_levels[2];
	func_out pte_valid(pte, pte_levels);

	output mem_adrs[34];
	input mem_rdata[32];
	func_out mem_read(mem_adrs);
	func_in mem_valid;
}
#endif
