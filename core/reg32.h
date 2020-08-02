#ifndef REG32_H
#define REG32_H
declare reg32 {
	/* written value is appeared on read port at same clock (forwarded) */
	input rnuma[5];
	input rnumb[5];
	input wnuma[5];
	input wdataa[32];
	output rdataa[32];
	output rdatab[32];
	func_in reada(rnuma) : rdataa;
	func_in readb(rnumb) : rdatab;
	func_in writea(wnuma, wdataa);
#ifdef DEBUG
	output debug_x0[32];
	output debug_x1[32];
	output debug_x2[32];
	output debug_x3[32];
	output debug_x4[32];
	output debug_x5[32];
	output debug_x6[32];
	output debug_x7[32];
	output debug_x8[32];
	output debug_x9[32];
	output debug_x10[32];
	output debug_x11[32];
	output debug_x12[32];
	output debug_x13[32];
	output debug_x14[32];
	output debug_x15[32];
	output debug_x16[32];
	output debug_x17[32];
	output debug_x18[32];
	output debug_x19[32];
	output debug_x20[32];
	output debug_x21[32];
	output debug_x22[32];
	output debug_x23[32];
	output debug_x24[32];
	output debug_x25[32];
	output debug_x26[32];
	output debug_x27[32];
	output debug_x28[32];
	output debug_x29[32];
	output debug_x30[32];
	output debug_x31[32];
#endif
}
#endif

