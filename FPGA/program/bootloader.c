

#define MMC_BASE 0x40001000
#define MMC_STATUS ((volatile unsigned int *)(MMC_BASE + 0x200))
#define MMC_R1STAT ((volatile unsigned int *)(MMC_BASE + 0x204))
#define MMC_RESP ((volatile unsigned int *)(MMC_BASE + 0x208))
#define MMC_CMD ((volatile unsigned int *)(MMC_BASE + 0x20C))
#define MMC_ARG ((volatile unsigned int *)(MMC_BASE + 0x210))
#define MMC_CMD_TYPE ((volatile unsigned int *)(MMC_BASE + 0x214))
#define MMC_DATA_BASE ((volatile unsigned int *)(MMC_BASE + 0x0))
#define INITED 0x1
#define IDLE 0x2
#define EXEC 0x4
#define INTR_EN 0x8
#define OK (INITED | IDLE)
#define MMC_RSP_SPI_MASK (15 << 7)
#define MMC_RSP_SPI_S1 (1 << 7)    /* one status byte */
#define MMC_RSP_SPI_S2 (1 << 8)    /* second byte */
#define MMC_RSP_SPI_B4 (1 << 9)    /* four data bytes */
#define MMC_RSP_SPI_BUSY (1 << 10) /* card may send busy */
#define MMC_RSP_SPI_R1 (MMC_RSP_SPI_S1)
#define MMC_RSP_SPI_R1B (MMC_RSP_SPI_S1 | MMC_RSP_SPI_BUSY)
#define MMC_RSP_SPI_R2 (MMC_RSP_SPI_S1 | MMC_RSP_SPI_S2)
#define MMC_RSP_SPI_R3 (MMC_RSP_SPI_S1 | MMC_RSP_SPI_B4)
#define MMC_CMD_AC (0 << 5)
#define MMC_CMD_ADTC (1 << 5)
#define MMC_CMD_BC (2 << 5)
#define MMC_CMD_BCR (3 << 5)
#define MMC_RECIEVE_BYTES_MASK 0xFFFF0000
#define MMC_RECIEVE_BYTES(u) (u << 16)
#define MMC_WRITE (1 << 0)
#define MMC_READ (1 << 1)

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
void exeCMD(unsigned, unsigned, unsigned);
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
        exeCMD(17, sector, MMC_RECIEVE_BYTES(512) | MMC_RSP_SPI_R1 | MMC_CMD_ADTC | MMC_READ);
        sector += SECTORSIZE;
        fetchSector(TEXTHEAD + 128 * offset++); // 4byte offset, 128*4=512
    }
}

void exeCMD(unsigned cmd, unsigned arg, unsigned cmd_types)
{
    spiWait();
    *MMC_CMD = cmd;
    *MMC_ARG = arg;
    *MMC_CMD_TYPE = cmd_types;
    *MMC_STATUS = *MMC_STATUS | EXEC;
    spiWait();
}