#ifndef UART_SENDER_H
#define UART_SENDER_H
#define TX_BAUD_RATE_9600 5208
#define TX_BAUD_RATE_76800 651
#define TX_BAUD_RATE_38400 1302
#define TX_BAUD_RATE_19200 2604
#define TX_BAUD_RATE_115200 434
#define TX_BAUD_RATE_CNT TX_BAUD_RATE_19200

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
}

struct uart_tx_stat_t {
	x[4];	
	full[1];
	empty[1];
	busy[1];
	en[1];
};
#endif
