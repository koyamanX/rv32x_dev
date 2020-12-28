#ifndef PLIC_H
#define PLIC_H
declare plic {
	/* To/from memory bus */
	input adrs[24];
	output rdata[32];
	input wdata[32];
	func_in read(adrs) : rdata;
	func_in write(adrs, wdata);
	func_out valid();
	
	/* interrupt request lines */
	func_in interrupt_req1;
	func_in interrupt_req2;
	func_in interrupt_req3;
	func_in interrupt_req4;
	func_in interrupt_req5;
	func_in interrupt_req6;
	func_in interrupt_req7;
	func_in interrupt_req8;
	func_in interrupt_req9;
	func_in interrupt_req10;
	func_in interrupt_req11;
	func_in interrupt_req12;
	func_in interrupt_req13;
	func_in interrupt_req14;
	func_in interrupt_req15;
	func_in interrupt_req16;
	func_in interrupt_req17;
	func_in interrupt_req18;
	func_in interrupt_req19;
	func_in interrupt_req20;
	func_in interrupt_req21;
	func_in interrupt_req22;
	func_in interrupt_req23;
	func_in interrupt_req24;
	func_in interrupt_req25;
	func_in interrupt_req26;
	func_in interrupt_req27;
	func_in interrupt_req28;
	func_in interrupt_req29;
	func_in interrupt_req30;
	func_in interrupt_req31;

	/* interrupt request to hart */
	func_out hart0_interrupt_reqeust();
	func_out hart1_interrupt_reqeust();
}

struct priority_t {
	//x[29];
	priority[3];
};
struct pending_t {
	//x[30];
	pending[1];
};
struct enable_t {
	//x[30];
	enable[1];
};
struct threshold_t {
	threshold[3];
};

/* 24 bit */
#define PLIC_PRIORITY_REGISTER_BASE 	0x00_0000
#define PLIC_PRIORITY_REGISTER_SIZE 	0x00_0080
#define PLIC_PRIORITY_REGISTER_LEN 		32
#define PLIC_PENDING_REGISTER_BASE		0x00_1000
#define PLIC_PENDING_REGISTER_SIZE		0x00_0004
#define PLIC_PENDING_REGISTER_LEN		1
#define PLIC_ENABLE_REGISTER_BASE		0x00_2000
#define PLIC_ENABLE_REGISTER_SIZE		0x00_0004
#define PLIC_ENABLE_REGISTER_LEN		1
#define PLIC_THRESHOLD_REGISTER_BASE	0x20_0000
#define PLIC_THRESHOLD_REGISTER_SIZE	0x00_0004
#define PLIC_THRESHOLD_REGISTER_LEN		1
#define PLIC_CLAIM_REGISTER_BASE		0x20_0004
#define PLIC_CLAIM_REGISTER_SIZE		0x00_0004
#define PLIC_CLAIM_REGISTER_LEN			1
#endif

