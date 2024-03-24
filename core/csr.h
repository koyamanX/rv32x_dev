#ifndef CSR_H
#define CSR_H

#include "priv.h"

/* TODO: cover all CSRs */
/* Register permission attributes */
#define ATTRIBUTE_URO
#define ATTRIBUTE_URW
#define ATTRIBUTE_SRO
#define ATTRIBUTE_SRW
#define ATTRIBUTE_MRO
#define ATTRIBUTE_MRW
/* Register feild attributes */
#define ATTRIBUTE_WPRI
#define ATTRIBUTE_WLRL
#define ATTRIBUTE_WARL
#define ATTRIBUTE_RDONLY
#define ATTRIBUTE_NONE

#define MISA_MXL_32          2'b01
#define MISA_MXL_64          2'b10
#define MISA_MXL_128         2'b11

#define MISA_EXTENSIONS_A    26'b00000_00000_00000_00000_00000_1
#define MISA_EXTENSIONS_B    26'b00000_00000_00000_00000_00001_0
#define MISA_EXTENSIONS_C    26'b00000_00000_00000_00000_00010_0
#define MISA_EXTENSIONS_D    26'b00000_00000_00000_00000_00100_0
#define MISA_EXTENSIONS_E    26'b00000_00000_00000_00000_01000_0
#define MISA_EXTENSIONS_F    26'b00000_00000_00000_00000_10000_0
#define MISA_EXTENSIONS_G    26'b00000_00000_00000_00001_00000_0
#define MISA_EXTENSIONS_H    26'b00000_00000_00000_00010_00000_0
#define MISA_EXTENSIONS_I    26'b00000_00000_00000_00100_00000_0
#define MISA_EXTENSIONS_J    26'b00000_00000_00000_01000_00000_0
#define MISA_EXTENSIONS_K    26'b00000_00000_00000_10000_00000_0
#define MISA_EXTENSIONS_L    26'b00000_00000_00001_00000_00000_0
#define MISA_EXTENSIONS_M    26'b00000_00000_00010_00000_00000_0
#define MISA_EXTENSIONS_N    26'b00000_00000_00100_00000_00000_0
#define MISA_EXTENSIONS_O    26'b00000_00000_01000_00000_00000_0
#define MISA_EXTENSIONS_P    26'b00000_00000_10000_00000_00000_0
#define MISA_EXTENSIONS_Q    26'b00000_00001_00000_00000_00000_0
#define MISA_EXTENSIONS_R    26'b00000_00010_00000_00000_00000_0
#define MISA_EXTENSIONS_S    26'b00000_00100_00000_00000_00000_0
#define MISA_EXTENSIONS_T    26'b00000_01000_00000_00000_00000_0
#define MISA_EXTENSIONS_U    26'b00000_10000_00000_00000_00000_0
#define MISA_EXTENSIONS_V    26'b00001_00000_00000_00000_00000_0
#define MISA_EXTENSIONS_W    26'b00010_00000_00000_00000_00000_0
#define MISA_EXTENSIONS_X    26'b00100_00000_00000_00000_00000_0
#define MISA_EXTENSIONS_Y    26'b01000_00000_00000_00000_00000_0
#define MISA_EXTENSIONS_Z    26'b10000_00000_00000_00000_00000_0

#define MTVEC_MODE_DIRECT    2'b00
#define MTVEC_MODE_VECTORED  2'b01

#define USER_SOFTWARE_INTERRUPT         0x8000_0000
#define SUPERVISOR_SOFTWARE_INTERRUPT   0x8000_0001
#define MACHINE_SOFTWARE_INTERRUPT      0x8000_0003
#define USER_TIMER_INTERRUPT            0x8000_0004
#define SUPERVISOR_TIMER_INTERRUPT      0x8000_0005
#define MACHINE_TIMER_INTERRUPT         0x8000_0007
#define USER_EXTERNAL_INTERRUPT         0x8000_0008
#define SUPERVISOR_EXTERNAL_INTERRUPT   0x8000_0009
#define MACHINE_EXTERNAL_INTERRUPT      0x8000_000b

#define INSTRUCTION_ADDRESS_MISALIGNED  4'b0000
#define INSTRUCTION_ACCESS_FAULT        4'b0001
#define ILLEGAL_INSTRUCTION             4'b0010
#define BREAKPOINT                      4'b0011
#define LOAD_ADDRESS_MISALIGNED         4'b0100
#define LOAD_ACCESS_FAULT               4'b0101
#define STORE_AMO_ADDRESS_MISALIGNED    4'b0110
#define STORE_AMO_ACCESS_FAULT          4'b0111
#define ENVIRONMENT_CALL_FROM_U_MODE    4'b1000
#define ENVIRONMENT_CALL_FROM_S_MODE    4'b1001
#define ENVIRONMENT_CALL_FROM_M_MODE    4'b1011
#define INSTRUCTION_PAGE_FAULT          4'b1100
#define LOAD_PAGE_FAULT                 4'b1101
#define STORE_AMO_PAGE_FAULT            4'b1111
#define RESET							0

/* MXLEN == 32 bit */


struct csr32_t {
	v[32];
};
struct csr64_t {
	v[64];
};
/* CSRs structures */
struct misa_t {
	mxl[2];
	reserved0[4];
	extensions[26];
};
struct mvendorid_t {
	bank[25];
	offset[7];
};
struct marchid_t {
	architecture_id[32];
};
struct mimpid_t {
	implementation[32];
};
struct mhartid_t {
	hart_id[32];
};
struct mstatus_t {
	sd[1];
	reserved0[8];
	tsr[1];
	tw[1];
	tvm[1];
	mxr[1];
	sum[1];
	mprv[1];
	xs[2];
	fs[2];
	mpp[2];
	reserved1[2];
	spp[1];
	mpie[1];
	ube[1];
	spie[1];
	upie[1];
	mie[1];
	reserved3[1];
	sie[1];
	uie[1];
};
#define sstatus_t mstatus_t
#define ustatus_t mstatus_t
struct mstatush_t {
	reserved0[26];
	mbe[1];
	sbe[1];
	reserved[4];
};
struct mtvec_t {
	base[30];
	mode[2];
};
#define stvec_t mtvec_t
#define utvec_t mtvec_t
struct mip_t {
	reserved0[20]; 
	meip[1];
	reserved1[1];
	seip[1];
	ueip[1];
	mtip[1];
	reserved3[1];
	stip[1];
	utip[1];
	msip[1];
	reserved5[1];
	ssip[1];
	usip[1];
};
struct mie_t {
	reserved0[20]; 
	meie[1];
	reserved1[1];
	seie[1];
	ueie[1];
	mtie[1];
	reserved3[1];
	stie[1];
	utie[1];
	msie[1];
	reserved5[1];
	ssie[1];
	usie[1];
};

#define sip_t mip_t
#define uip_t mip_t
#define sie_t mie_t
#define uie_t mie_t
struct mcounteren_t {
	hpm31[1];
	hpm30[1];
	hpm29[1];
	hpm28[1];
	hpm27[1];
	hpm26[1];
	hpm25[1];
	hpm24[1];
	hpm23[1];
	hpm22[1];
	hpm21[1];
	hpm20[1];
	hpm19[1];
	hpm18[1];
	hpm17[1];
	hpm16[1];
	hpm15[1];
	hpm14[1];
	hpm13[1];
	hpm12[1];
	hpm11[1];
	hpm10[1];
	hpm9[1];
	hpm8[1];
	hpm7[1];
	hpm6[1];
	hpm5[1];
	hpm4[1];
	hpm3[1];
	ir[1];
	tm[1];  /* if mcountinhibit, tm is hardwired to zero */
	cy[1];
};
#define mcountinhibit_t mcounteren_t
struct mcause_t {
	interrupt[1];
	reserved[21];
	exception_code[10];
};
#define scause_t mcause_t
#define ucause_t mcause_t

#define mscratch_t csr32_t
#define sscratch_t csr32_t
#define uscratch_t mscratch_t
#define mepc_t csr32_t
#define sepc_t csr32_t
#define uepc_t mepc_t
#define mtval_t csr32_t
#define stval_t csr32_t
#define utval_t mtval_t
struct medeleg_t {
	reserved2[16];
	store_amo_page_fault;
	reserved1[1];
	load_page_fault;
	instruction_page_fault;
	environment_call_from_m_mode;
	reserved0[1];
	environment_call_from_s_mode;
	environment_call_from_u_mode;
	store_amo_access_fault;
	store_amo_address_misaligned;
	load_access_fault;
	load_address_misaligned;
	breakpoint;
	illegal_instruction;
	instruction_access_fault;
	instruction_address_misaligned;
};
#define mideleg_t mip_t
struct rv32_satp_t {
	mode[1];
	asid[9];
	ppn[22];
};
#define satp_t rv32_satp_t
struct sv32_va_t {
	vpn1[10];
	vpn0[10];
	page_offset[12];
};
struct sv32_pa_t {
	ppn1[12];
	ppn0[10];
	page_offset[12];
};
struct sv32_pte_t {
	ppn1[12];
	ppn0[10];
	rsw[2];
	d;
	a;
	g;
	u;
	x;
	w;
	r;
	v;
};

#define SATP_32_MODE_BARE 1'b0
#define SATP_32_MODE_SV32 1'b1
#define INIT_MTVEC {30'b00000_00000_00000_00000_00000_00000, MTVEC_MODE_DIRECT}
#define MISA_EXTENSIONS MISA_EXTENSIONS_I|MISA_EXTENSIONS_M|MISA_EXTENSIONS_A|MISA_EXTENSIONS_U|MISA_EXTENSIONS_S

declare csr32{
	output mip_[32];
	output mie_[32];
	output sie_[32];
	output satp_[32];
	output mstatus_[32];
	output sstatus_[32];
	output mideleg_[32];
	output medeleg_[32];
	input priv_mode[2];
	input meip;
	input mtip;
	input msip;
	input seip;
	input stip;
	input ssip;
	input ialign[2];
	input instret;
	func_in reset();
	input cradrs[32];
	output crdata[32];
	func_in read(cradrs) : crdata;
	input cwadrs[32];
	input cwdata[32];
	func_in write(cwadrs, cwdata);
	func_out read_not_mapped();
	func_out write_not_mapped();
	input mode[2];
	output epc[32];
	func_in trap_ret(mode) : epc;
	input trap_priv_mode[2];
	input trap_cause[32];
	input trap_pc[32];
	input trap_val[32];
	output tvec_[32];
	func_in trap(trap_priv_mode, trap_cause, trap_pc, trap_val) : tvec_;
}

#endif
