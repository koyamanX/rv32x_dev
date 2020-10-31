#ifndef PTW_H
#define PTW_H
declare ptw {
	func_in reset();
	input satp[32];
	input virtual_address[32];
	func_in walk(virtual_address);
	func_out page_fault();
	output page_table_entry[32];
	func_out pte_valid(page_table_entry);
	output mem_adrs[34];
	input mem_rdata[32];
	func_out mem_read(mem_adrs);
	func_in mem_valid;
}
#endif
