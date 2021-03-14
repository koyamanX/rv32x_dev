#ifndef LOAD_STORE_UNIT_H
#define LOAD_STORE_UNIT_H

#define CLEAN 1'b0
#define DIRTY 1'b1
#define VALID 1'b1
#define INVALID 1'b0

#define CACHE_WRITE	1'b1
#define CACHE_READ	1'b0

struct reservation_t {
	valid;
	addr[32];
};

declare load_store_unit {
	/* CPU operations (Virtual Address) */
	func_in reset();
	input addr[32];
	input byteen[3];
	output load_data[32];
	input store_data[32];
	func_in load(addr, byteen);					/* must be asserted until valid arrives */
	func_in store(addr, byteen, store_data);	/* must be asserted until valid arrives */
	func_in load_reserved();					/* This signal is asserted alongside with load */
	func_in store_conditional();				/* This signal is asserted alongside with store */
	func_out valid;								/* operation is done */

	input amo_src[32];
	input amo_addr[32];
	input amo_funct5[5];
	input amo_aq;
	input amo_rl;
	input amo_width[3];
	func_in amo_exe(amo_src, amo_addr, amo_funct5, amo_aq, amo_rl, amo_width);

	func_in cache_flush;
	func_out cache_flush_done;
	func_in tlb_flush();
	func_out tlb_flush_done();

	/* CSRs(read-only) */
	input satp_mode;
	input satp_asid[9];
	input satp_ppn[22];
	input mstatus_mxr;
	input priv_mode[2];
	input sstatus_sum;
	
	/* Exceptions to CPU */
	func_out store_amo_page_fault();
	func_out store_amo_access_fault();
	func_out load_page_fault();
	func_out load_access_fault();

	/* uTLB I/F */
	input tlb_addr[32];
	func_in read_tlb(tlb_addr);
	output pte[32];
	output pte_levels[2];
	func_out pte_ready();
	func_out tlb_page_fault();
	func_out tlb_access_fault();

	/* PMAs */
	output pma_addr[32];
	func_out pma_check(pma_addr);
	func_in pma_writable();
	func_in pma_readable();
	func_in pma_atomic();
	func_in pma_word();
	func_in pma_halfword();
	func_in pma_byte();
	func_in pma_cacheable();
	func_in pma_empty();

	/* Pysical Address */
	/* D-BUS(Cacheable, may support burst access with maximum word width of 128 bits) */
	output dbus_addr[32];
	input dbus_rdata[128];
	output dbus_wdata[128];
	output dbus_size[3];
	func_out dbus_read(dbus_addr, dbus_size);
	func_out dbus_write(dbus_addr, dbus_size, dbus_wdata);
	/* Locked transfer */
	func_out dbus_locked();
	func_in dbus_ready;
	func_in dbus_load_access_fault();
	func_in dbus_store_amo_access_fault();
	/* Flush buffer, cache in D-BUS if installed */
	func_out dbus_flush();
	func_in dbus_flush_done();

	/* Pysical Address */
	/* P-BUS(Noncacheable, no burst access, maximum word width of 32 bits, may support byte, halfword access) */
	output pbus_addr[32];
	input pbus_rdata[32];
	output pbus_wdata[32];
	output pbus_size[3];
	func_out pbus_read(pbus_addr, pbus_size);
	func_out pbus_write(pbus_addr, pbus_size, pbus_wdata);
	/* Locked transfer */
	func_out pbus_locked();
	/* P-BUS status */
	func_in pbus_ready();
	func_in pbus_load_access_fault();
	func_in pbus_store_access_fault();
	func_out pbus_flush();
	func_in pbus_flush_done();

#ifdef DEBUG
	output tohost[32];
	func_out sim_done(tohost);
#endif
}
#endif
