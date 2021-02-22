#ifndef UART_RECEIVER_H
#define UART_RECEIVER_H
#define RX_BAUD_RATE_9600 5208
#define RX_BAUD_RATE_76800 651
#define RX_BAUD_RATE_38400 1302
#define RX_BAUD_RATE_19200 2604
#define RX_BAUD_RATE_115200 434
#define RX_BAUD_RATE_CNT (RX_BAUD_RATE_38400/8)

declare uart_receiver {
	input addr[10];
	output rdata[32];
	input wdata[32];
	func_in read(addr);
	func_in write(addr, wdata);
	func_in reset();
	func_out ready();
	func_out done();
	func_out interrupt_req;
	output received_char[8];
	input RXD;
}

struct uart_rx_stat_t {
	x[4];
	full[1];
	empty[1];
	busy[1];
	en[1];
};
#endif
