#ifndef UART_SENDER_H
#define UART_SENDER_H
#define TX_BAUD_RATE_9600 5208
#define TX_BAUD_RATE_76800 651
#define TX_BAUD_RATE_38400 1302
#define TX_BAUD_RATE_19200 2604
#define TX_BAUD_RATE_115200 434
#define TX_BAUD_RATE_CNT TX_BAUD_RATE_38400

declare uart_sender {
	input addr[10];
	output rdata[32];
	input wdata[32];
	func_in read(addr);
	func_in write(addr, wdata);
	func_out ready();
	func_in reset();
	func_out done();
	func_out interrupt_req();
	output TXD;
}

struct uart_tx_stat_t {
	x[4];	
	full[1];
	empty[1];
	busy[1];
	en[1];
};
#endif
