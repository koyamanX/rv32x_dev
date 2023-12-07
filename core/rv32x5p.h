#ifndef RV32X5P_H
#define RV32X5P_H
declare rv32x5p
{
	input resetvector[32];						/* Initial PC value on reset */
	func_in reset(resetvector);					/* Reset processor */
	input ialign[2];							/* ialign (32==2'b00, 16==2'b10) */
	input insts[128];							/* Irnstruction read on imem_read, only valid during imem_ready */
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
	func_out mret(); /* Indicating mret is issued */
	func_out sret(); /* Indicating sret is issued */
	func_out wfi();			/* indicating Wait-For-Interrupt is issued */
	input ifetch_exception[5];
	input execute_exception[5];
	input memory_exception[5];
	output exception_cause[4];
	func_out exception_req(exception_cause);
	func_out trap_mret();
	func_out trap_sret();

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

	func_out validate_pipeline(); // 現在のパイプラインコンテキストが放棄されたものでないことを保証する(連続でtrapが起こった時、不正なepcを入れないようにするため)

	/* Instruction is retired, increment instret register */
	func_out instret();
#ifdef CV
	input SW[10];
	output pc_led[10];
	func_out debug_pc_led(pc_led);
	output sseg_l16[16]; // execute stage pc low 16bit
	func_out debug_sseg_l16(sseg_l16);
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
