
#define SD_BASE 0x40001000
#define SD_STATUS ((volatile unsigned int *)(SD_BASE + 0x200))
#define SD_ADRS ((volatile unsigned int *)(SD_BASE + 0x214))
#define SD_OP ((volatile unsigned int *)(SD_BASE + 0x210))
#define SD_DATA_BASE ((volatile unsigned int *)(SD_BASE + 0x0))
#define SECTORSIZE 512
#define TEXTHEAD ((volatile unsigned int *)0x80000000)

void spiWait();
void cpyToMem(volatile unsigned int *);
void readBlock(unsigned int);
void writeBlock(volatile unsigned int *, unsigned int);

void main(void)
{
    spiWait();
    readBlock(0);
    cpyToMem((volatile unsigned int *)0x80004000); // 4byte offset, 128*4=512
    *((volatile unsigned int *)0x80004000) = 0xDEADBEEF;
    writeBlock((volatile unsigned int *)0x80004000, 0);
    while (1)
    {
    }
}

void spiWait()
{
    while ((*SD_STATUS) != 0x3)
    {
        asm volatile("nop");
    }
}

void cpyToMem(volatile unsigned int *memaddr)
{
    for (int i = 0; i < SECTORSIZE; i += 4)
    {
        *(memaddr + (i >> 2)) = SD_DATA_BASE[127 - (i >> 2)]; // 4byte offset
    }
}

void readBlock(unsigned int sector)
{
    *SD_ADRS = sector;
    *SD_OP = 0x00000001;
    spiWait();
}

void writeBlock(volatile unsigned int *memaddr, unsigned int sector)
{
    for (int i = 0; i < SECTORSIZE; i += 4)
    {
        SD_DATA_BASE[127 - (i >> 2)] = *(memaddr + (i >> 2)); // 4byte offset
    }
    *SD_ADRS = sector;
    *SD_OP = 0x00000002;
    spiWait();
}