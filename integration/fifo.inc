
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

