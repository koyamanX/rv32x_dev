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

module fifo_%WIDTH%_%DEPTH% {
	mem ram[MEMDEPTH][WIDTH];
	reg wptr[DEPTH] = 0;
	reg rptr[DEPTH] = 0;
	func_self e;
	func_self f;

	func write {
		if(!f) {
			ram[wptr] := wdata;
			wptr := wptr + DEPTH'(1'b1);
		}
	}
	func read {
		if(!e) {
			rptr := rptr + DEPTH'(1'b1);
			return ram[rptr];
		}
	}
	if((wptr + DEPTH'(1'b1)) == rptr) {
		f();
		full();
	}
	if(wptr == rptr) {
		e();
		empty();
	}
}

#undef WIDTH
#undef DEPTH
#undef MEMDEPTH
