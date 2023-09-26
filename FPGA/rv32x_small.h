
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
}
