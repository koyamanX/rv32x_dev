#ifndef WIDTH
#define WIDTH 8
#endif

#ifndef DEPTH
#define DEPTH 8
#endif

#define MEMDEPTH _int(_pow(2.0, DEPTH.0))


declare fifo_%WIDTH%_%DEPTH% {
	input wdata[WIDTH];
	output rdata[WIDTH];
	func_in write(wdata);
	func_in read() : rdata;
	func_out empty;
	func_out full;
}

