#ifndef UART_RECEIVER_H
#define UART_RECEIVER_H
#define RX_BAUD_RATE_9600 5208
#define RX_BAUD_RATE_76800 651
#define RX_BAUD_RATE_38400 1302
#define RX_BAUD_RATE_19200 2604
#define RX_BAUD_RATE_115200 434
#define RX_BAUD_RATE_CNT (RX_BAUD_RATE_19200/8)

declare uart_receiver
{
	input RXD;
	input adrs[10];
	input wdata[32];
	output rdata[32];
	output received_char[8];
	func_in read(adrs) : rdata;
	func_in write(adrs, wdata);
	func_in reset();
	func_out done();
	func_out valid();
	func_out int_req;
}

struct uart_rx_stat_t {
	x[6];
	busy[1];
	en[1];
};
#endif
