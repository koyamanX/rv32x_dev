#ifndef UART_RECEIVER_H
#define UART_RECEIVER_H

#ifndef CPU_FREQ
#define CPU_FREQ 50.0
#endif

#define RX_BAUD_RATE_9600 _int(CPU_FREQ * 1000000.0 / 9600.0)
#define RX_BAUD_RATE_19200 _int(CPU_FREQ * 1000000.0 / 19200.0)
#define RX_BAUD_RATE_38400 _int(CPU_FREQ * 1000000.0 / 38400.0)
#define RX_BAUD_RATE_76800 _int(CPU_FREQ * 1000000.0 / 76800.0)
#define RX_BAUD_RATE_115200 _int(CPU_FREQ * 1000000.0 / 115200.0)
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
