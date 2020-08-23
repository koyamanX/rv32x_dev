#ifndef UART_SENDER_H
#define UART_SENDER_H
#define TX_BAUD_RATE_CNT 5208

declare uart_sender {
	input adrs[10];
	input wdata[32];
	output rdata[32];
	output TXD;
	func_in read(adrs) : rdata;
	func_in write(adrs, wdata);
	func_out valid();
	func_in reset();
	func_out done();
	func_out int_req();
}

struct uart_tx_stat_t {
	x[6];	
	busy[1];
	en[1];
};
#endif
