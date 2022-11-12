
declare rv32x_small
{
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
    func_out imem_read(iaddr, ibyteen);         /* When data is ready on 'inst', imem_ready is asserted */
    func_out dmem_read(daddr, dbyteen);         /* When data is ready on 'rdata', dmem_ready is asserted */
    func_out dmem_write(daddr, dbyteen, wdata); /* When data is written, dmem_ready is asserted */

#ifdef RISCV_TESTS
    output tohost[32];
    func_out sim_done(tohost);
#endif
#ifdef CV
    input SW[10];
    output pc_led[10];
    func_out debug_pc_led(pc_led);
    output sseg_h8[8];
    func_out debug_sseg_h8(sseg_h8);
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
