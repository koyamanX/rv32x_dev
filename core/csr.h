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

#define USER_SOFTWARE_INTERRUPT         0 
#define SUPERVISOR_SOFTWARE_INTERRUPT   1 
#define MACHINE_SOFTWARE_INTERRUPT      3
#define USER_TIMER_INTERRUPT            4
#define SUPERVISOR_TIMER_INTERRUPT      5
#define MACHINE_TIMER_INTERRUPT         7
#define USER_EXTERNAL_INTERRUPT         8
#define SUPERVISOR_EXTERNAL_INTERRUPT   9
#define MACHINE_EXTERNAL_INTERRUPT      11

#define INSTRUCTION_ADDRESS_MISALIGNED 0
#define INSTRUCTION_ACCESS_FAULT        1
#define ILLEGAL_INSTRUCTION             2
#define BREAKPOINT                      3
#define LOAD_ADDRESS_MISALIGNED         4
#define LOAD_ACCESS_FAULT               5
#define STORE_AMO_ADDRESS_MISALIGNED    6
#define STORE_AMO_ACCESS_FAULT          7
#define ENVIRONMENT_CALL_FROM_U_MODE    8
#define ENVIRONMENT_CALL_FROM_S_MODE    9
#define ENVIRONMENT_CALL_FROM_M_MODE    11
#define INSTRUCTION_PAGE_FAULT          12
#define LOAD_PAGE_FAULT                 13
#define STORE_AMO_PAGE_FAULT            15
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
	reserved2[1];
	spie[1];
	upie[1];
	mie[1];
	reserved3[1];
	sie[1];
	uie[1];
};
struct mtvec_t {
	base[30];
	mode[2];
};
struct mip_t {
	reserved0[19]; meip[1];
	reserved1[1];
	seip[1];
	ueip[1];
	mtip[1];
	reserved2[1];
	stip[1];
	utip[1];
	msip[1];
	reserved3[1];
	ssip[1];
	usip[1];
};
#define mie_t mip_t
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
	exception_code[31];
};

#define mscratch_t csr32_t
#define mepc_t csr32_t
#define mtval_t csr32_t
#endif
