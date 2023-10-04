#ifndef RV32X_CORE_H
#define RV32X_CORE_H
declare rv32x_core
{
	func_in reset();

	/* I-BUS */
	output iaddr[32];
	input inst[32];
	output ibyteen[3];
	func_out imem_read(iaddr, ibyteen);
	func_in imem_ready;

	/* D-BUS */
	output daddr[32];
	input rdata[32];
	output wdata[32];
	output dbyteen[3];
	func_out dmem_read(daddr, dbyteen);
	func_out dmem_write(daddr, dbyteen, wdata);
	func_in dmem_ready;

	/* PMAs */
	output imem_pma_addr[32];
	func_out imem_pma_check(imem_pma_addr);
	func_in imem_pma_executable();
	func_in imem_pma_readable();
	func_in imem_pma_word();
	func_in imem_pma_halfword();
	func_in imem_pma_cacheable();
	func_in imem_pma_empty();

	output dmem_pma_addr[32];
	func_out dmem_pma_check(dmem_pma_addr);
	func_in dmem_pma_writable();
	func_in dmem_pma_readable();
	func_in dmem_pma_atomic();
	func_in dmem_pma_word();
	func_in dmem_pma_halfword();
	func_in dmem_pma_byte();
	func_in dmem_pma_cacheable();
	func_in dmem_pma_empty();

	/* Interrupts */
	func_in machine_timer_interrupt_req();
	func_in machine_software_interrupt_req();
	func_in machine_external_interrupt_req();
	func_in supervisor_timer_interrupt_req();
	func_in supervisor_software_interrupt_req();
	func_in supervisor_external_interrupt_req();
#ifdef RISCV_TESTS
	output tohost[32];
	func_out sim_done(tohost);
#endif
#ifdef CV
	input SW[10];
	output pc_led[10];
	func_out debug_pc_led(pc_led);
	output sseg_h8[8];
	func_out debug_sseg_h8(sseg_h8); // mcause
	output sseg_l16[16];
	func_out debug_sseg_l16(sseg_l16);
	output sseg_l12[12];
	func_out debug_sseg_l12(sseg_l12);
	output sseg_h12[12];
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
