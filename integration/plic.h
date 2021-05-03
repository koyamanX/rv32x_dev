#ifndef PLIC_H
#define PLIC_H
#define PLIC_BASE_ADDR 0x0c000000
#define PLIC_PRIORITY_BASE_ADDR 0x0c000000
#define PLIC_PENDING_BASE_ADDR 0x0c001000
#define PLIC_ENABLE_BASE_ADDR 0x0c002000
#define PLIC_THRESHOLD_BASE_ADDR 0x0c200000
#define PLIC_CLAIM_COMPLETE_BASE_ADDR 0x0c200004
#define PLIC_SIZE 0x10000000
declare plic {
	input addr[32];
	input wdata[32];
	output rdata[32];
	func_in read(addr);
	func_in write(addr, wdata);
	func_in reset();
	func_out ready();
	func_out external_interrupt_hart_0();
	func_out external_interrupt_hart_1();
	func_in irq_1;
	func_in irq_2;
	func_in irq_3;
	func_in irq_4;
	func_in irq_5;
	func_in irq_6;
	func_in irq_7;
	func_in irq_8;
	func_in irq_9;
	func_in irq_10;
	func_in irq_11;
	func_in irq_12;
	func_in irq_13;
	func_in irq_14;
	func_in irq_15;
	func_in irq_16;
	func_in irq_17;
	func_in irq_18;
	func_in irq_19;
	func_in irq_20;
	func_in irq_21;
	func_in irq_22;
	func_in irq_23;
	func_in irq_24;
	func_in irq_25;
	func_in irq_26;
	func_in irq_27;
	func_in irq_28;
	func_in irq_29;
	func_in irq_30;
	func_in irq_31;
}
struct pending_t {
	pending[1];
};
struct gateway_t {
	gateway[1];
};
#endif
