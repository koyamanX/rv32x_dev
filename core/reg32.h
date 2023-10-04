#ifndef REG32_H
#define REG32_H
declare reg32
{
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
}
#endif
