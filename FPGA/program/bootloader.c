
#define MMC_BASE 0x40001000
#define MMC_STATUS ((volatile unsigned int *)(MMC_BASE + 0x200))
#define MMC_R1STAT ((volatile unsigned int *)(MMC_BASE + 0x204))
#define MMC_RESP ((volatile unsigned int *)(MMC_BASE + 0x208))
#define MMC_CMD ((volatile unsigned int *)(MMC_BASE + 0x20C))
#define MMC_ARG ((volatile unsigned int *)(MMC_BASE + 0x210))
#define MMC_CD_BASE ((volatile unsigned int *)(MMC_BASE + 0x214)) // csd cid
#define MMC_DATA_BASE ((volatile unsigned int *)(MMC_BASE + 0x0))
#define INITED 0x1
#define IDLE 0x2
#define EXEC 0x4
#define INTR_EN 0x8
#define OK (INITED | IDLE)
#define R1 1
#define R1b 1
#define R2 2
#define R3 3
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
    asm volatile("fence.i");
    asm volatile("lui ra, %0"
                 :
                 : "i"((unsigned int)(TEXTHEAD) >> 12));
    asm volatile("ret");
}

void spiWait()
{
    while ((*MMC_STATUS) != OK)
    {
        asm volatile("nop");
    }
}

void fetchSector(volatile unsigned int *memaddr)
{
    for (int i = 0; i < SECTORSIZE; i += 4)
    {
        *(memaddr + (i >> 2)) = MMC_DATA_BASE[0 + (i >> 2)]; // 4byte offset
    }
}

void loadBin(unsigned int sector)
{
    unsigned int offset = 0;
    while ((sector - BINSTART) < BINSIZE)
    {
        spiWait();
        *MMC_CMD = 17;
        *MMC_ARG = sector;
        *MMC_STATUS = *MMC_STATUS | EXEC;
        sector += SECTORSIZE;
        spiWait();
        fetchSector(TEXTHEAD + 128 * offset++); // 4byte offset, 128*4=512
    }
}