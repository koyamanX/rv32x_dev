#ifndef RV32X5P_H
#define RV32X5P_H
declare rv32x5p
{
	input resetvector[32];						/* Initial PC value on reset */
	func_in reset(resetvector);					/* Reset processor */
	input ialign[2];							/* ialign (32==2'b00, 16==2'b10) */
	input inst[32];								/* Irnstruction read on imem_read, only valid during imem_ready */
	input rdata[32];							/* Data read on dmem_read, only valid during dmem_ready */
	output wdata[32];							/* Write data to dmem */
	output daddr[32];							/* Address to dmem */
	output iaddr[32];							/* Address to imem */
	output ibyteen[3];							/* Byte enable of imem */
	output dbyteen[3];							/* Byte enable of dmem */
	output cradrs[12];							/* CSR address for read */
	output cwadrs[12];							/* CSR address for write */
	input crdata[32];							/* Data read  */
	output cwdata[32];							/* Data to be written */
	func_in dmem_ready;							/* Indication of completation of current dmem transfer */
	func_in imem_ready;							/* Indication of completation of current imem transfer */
	func_out imem_read(iaddr, ibyteen);			/* When data is ready on 'inst', imem_valid is asserted */
	func_out dmem_read(daddr, dbyteen);			/* When data is ready on 'rdata', dmem_ready is asserted */
	func_out dmem_write(daddr, dbyteen, wdata); /* When data is written, dmem_ready is asserted */
	func_out csr_write_uimm();
	func_out csr_read(cradrs) : crdata; /* CSR read */
	func_out csr_write(cwadrs, cwdata); /* CSR write */
	func_out load_reserved();
	func_out store_conditional();
	output amo_src[32];
	output amo_addr[32];
	output amo_funct5[5];
	output amo_aq;
	output amo_rl;
	output amo_width[3];
	func_out amo_exe(amo_src, amo_addr, amo_funct5, amo_aq, amo_rl, amo_width, daddr);
	func_out fencei(); /* Indicating fence.i is issued */
	func_out fence();  /* Indicating fence is issued */
	output asid[9];
	output vaddr[32];
	func_out sfence_vma(vaddr, asid); /* Indicating sfence_vma is issued */
	func_out ecall();				  /* Indicating ecall is issued */
	func_out ebreak();				  /* Indicating ebreak is issued */
	input mepc[32];
	func_out mret() : mepc; /* Indicating mret is issued */
	input sepc[32];
	func_out sret() : sepc; /* Indicating sret is issued */
	input uepc[32];
	func_out uret() : uepc; /* Indicating uret is issued */
	func_out wfi();			/* indicating Wait-For-Interrupt is issued */

	/* Trap */
	input vector[32]; /* New PC on trap */
	output epc[32];	  /* Exception PC */
	func_in trap(vector);
	func_in flush_ifetch;
	func_in flush_decode;
	func_in flush_execute;
	func_in flush_memory;
	func_in stall_ifetch_req();
	func_in stall_decode_req();
	func_in stall_execute_req();
	func_in stall_memory_req();
	/* Internal exception status */
	output misaligned_target_address[32];
	output misaligned_memory_address[32];
	func_out instruction_address_misaligned(misaligned_target_address);
	func_out store_address_misaligned(misaligned_memory_address);
	func_out load_address_misaligned(misaligned_memory_address);
	func_out illegal_instruction_execute_stage();
	/* Get internal PC at the stage */
	func_in get_pc_ifetch() : epc;
	func_in get_pc_decode() : epc;
	func_in get_pc_execute() : epc;
	func_in get_pc_memory() : epc;
	func_in get_pc_trap() : epc;
	/* Get internal instruction at the stage */
	output einst[32]; /* Exception instruction */
	func_in get_inst_execute() : einst;
	func_in get_inst_memory() : einst;

	func_out valid_context(); //現在のパイプラインコンテキストが放棄されたものでないことを保証する(連続でtrapが起こった時、不正なepcを入れないようにするため)

	/* Instruction is retired, increment instret register */
	func_out instret();
#ifdef DEBUG
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
	output debug_wb_data[32];
	func_out debug_wb(debug_wb_rd, debug_wb_data);
	output debug_mem_adrs[32];
	output debug_mem_data[32];
	output debug_mem_byteen[3];
	func_out debug_mem_write(debug_mem_adrs, debug_mem_byteen, debug_mem_data);
	output debug_csr_write_num[12];
	output debug_csr_write_data[32];
	func_out debug_csr_write(debug_csr_write_num, debug_csr_write_data);
	input debug_cwadrs[12];
	input debug_cwdata[32];
	func_in get_csr(debug_cwadrs, debug_cwdata);
#endif
#ifdef CV
	func_out debug_led_1();
	func_out debug_led_2();
	func_out debug_led_3();
	func_out debug_led_4();
	func_out debug_led_5();
	func_out debug_led_6();
	func_out debug_led_7();
	func_out debug_led_8();
#endif
}
#endif
