
#define SD_BASE 0x40001000
#define SD_STATUS ((volatile unsigned int *)(SD_BASE + 0x200))
#define SD_ADRS ((volatile unsigned int *)(SD_BASE + 0x214))
#define SD_OP ((volatile unsigned int *)(SD_BASE + 0x210))
#define SD_DATA_BASE ((volatile unsigned int *)(SD_BASE + 0x0))
#define SECTORSIZE 512
#ifndef BINSTART
#define BINSTART 0
#endif
#ifdef HEAD
#define TEXTHEAD ((volatile unsigned int *)HEAD)
#else
#define TEXTHEAD ((volatile unsigned int *)0x80000000)
#endif

extern void _boot();
void spiWait();
void fetchSector(volatile unsigned int *);
void loadBin(unsigned int);

void main(void)
{
    unsigned int sector = BINSTART;
#if BINSIZE != 0
    spiWait();
    loadBin(sector);
#endif
    asm volatile("lui ra, %0"
                 :
                 : "i"((unsigned int)(TEXTHEAD) >> 12));
    asm volatile("ret");
}

void spiWait()
{
    while ((*SD_STATUS) != 0x3)
    {
        asm volatile("nop");
    }
}

void fetchSector(volatile unsigned int *memaddr)
{
    for (int i = 0; i < SECTORSIZE; i += 4)
    {
        *(memaddr + (i >> 2)) = SD_DATA_BASE[0 + (i >> 2)]; // 4byte offset
    }
}

void loadBin(unsigned int sector)
{
    unsigned int offset = 0;
    while (((sector - BINSTART) * SECTORSIZE) < BINSIZE)
    {
        *SD_ADRS = sector++;
        *SD_OP = 0x00000001;
        spiWait();
        fetchSector(TEXTHEAD + 128 * offset++); // 4byte offset, 128*4=512
    }
}