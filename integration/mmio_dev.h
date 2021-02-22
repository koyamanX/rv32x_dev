#ifndef MMIO_DEV_H
#define MMIO_DEV_H
declare mmio_dev {
	input addr[16];
	output rdata[32];
	input wdata[32];
	func_in read(addr);
	func_in write(addr, wdata);
	func_out ready();

	func_in reset();
	func_out uart_sender_interrupt_req();
	func_out uart_receiver_interrupt_req();

	output TXD;
	input RXD;
}
#endif
