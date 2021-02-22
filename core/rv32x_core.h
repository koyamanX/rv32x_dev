#ifndef RV32X_CORE_H
#define RV32X_CORE_H
declare rv32x_core {
	input inst[32];
	input rdata[32];
	output wdata[32];
	output daddr[32];
	output iaddr[32];
	output ibyteen[3];
	output dbyteen[3];
	func_in reset();
	func_in dmem_ready;
	func_in imem_ready;
	func_out imem_read(iaddr, ibyteen);  /* When data is ready on 'inst', imem_ready is asserted */
	func_out dmem_read(daddr, dbyteen);  /* When data is ready on 'rdata', dmem_ready is asserted */
	func_out dmem_write(daddr, dbyteen, wdata); /* When data is written, dmem_ready is asserted */
	output dmem_pma_addr[32];
	func_out pma_check_dmem(dmem_pma_addr);
	func_in dmem_cacheable();
	func_in dmem_empty();
	func_in machine_timer_interrupt_req();
	func_in machine_software_interrupt_req();
	func_in machine_external_interrupt_req();
	func_in supervisor_timer_interrupt_req();
	func_in supervisor_software_interrupt_req();
	func_in supervisor_external_interrupt_req();
#ifdef DEBUG
	output tohost[32];
	func_out sim_done(tohost);
	output debug_x0[32];
	output debug_x1[32];
	output debug_x2[32];
	output debug_x3[32];
	output debug_x4[32];
	output debug_x5[32];
	output debug_x6[32];
	output debug_x7[32];
	output debug_x8[32];
	output debug_x9[32];
	output debug_x10[32];
	output debug_x11[32];
	output debug_x12[32];
	output debug_x13[32];
	output debug_x14[32];
	output debug_x15[32];
	output debug_x16[32];
	output debug_x17[32];
	output debug_x18[32];
	output debug_x19[32];
	output debug_x20[32];
	output debug_x21[32];
	output debug_x22[32];
	output debug_x23[32];
	output debug_x24[32];
	output debug_x25[32];
	output debug_x26[32];
	output debug_x27[32];
	output debug_x28[32];
	output debug_x29[32];
	output debug_x30[32];
	output debug_x31[32];
	output debug_pc[32];
	output debug_retire_inst[32];
	output debug_retire_pc[32];
	func_out debug_retire(debug_retire_pc, debug_retire_inst);
	output debug_wb_rd[5];
	output debug_wb_value[32];
	func_out debug_wb(debug_wb_rd, debug_wb_value);
	output debug_csr_write_num[12];
	output debug_csr_write_data[32];
	func_out debug_csr_write(debug_csr_write_num, debug_csr_write_data);
	output debug_mem_adrs[32];
	output debug_mem_data[32];
	output debug_mem_byteen[3];
	func_out debug_mem_write(debug_mem_adrs, debug_mem_byteen, debug_mem_data);
	output debug_cause[32];
	output debug_epc[32];
	output debug_mtval[32];
	output debug_inst[32];
	func_out debug_raise_exception(debug_cause, debug_epc, debug_mtval, debug_inst);
#endif
}
#endif
