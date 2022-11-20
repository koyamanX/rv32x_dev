
#define SD_BASE 0x40001000
#define SD_STATUS ((volatile unsigned int *)(SD_BASE + 0x200))
#define SD_ADRS ((volatile unsigned int *)(SD_BASE + 0x214))
#define SD_OP ((volatile unsigned int *)(SD_BASE + 0x210))
#define SD_DATA_BASE ((volatile unsigned int *)(SD_BASE + 0x0))
#define SECTORSIZE 512
#define TEXTHEAD ((volatile unsigned int *)0x80000000)

void spiWait();
void writeBin(volatile unsigned char *);
void writeBlock(volatile unsigned int *, unsigned int);

/* Import a binary file */
#define IMPORT_BIN(sect, file, sym) asm(                                    \
    ".section " #sect "\n"                  /* Change section */            \
    ".balign 4\n"                           /* Word alignment */            \
    ".global " #sym "\n"                    /* Export the object address */ \
    #sym ":\n"                              /* Define the object label */   \
    ".incbin \"" file "\"\n"                /* Import the file */           \
    ".global _sizeof_" #sym "\n"            /* Export the object size */    \
    ".set _sizeof_" #sym ", . - " #sym "\n" /* Define the object size */    \
    ".balign 4\n")                          /* Word alignment */

extern const int bin[];
extern const char _sizeof_bin[];

void main(void)
{
    spiWait();

    writeBin((volatile unsigned char *)0x80001000);
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

void writeBin(volatile unsigned char *start)
{
    int sector = 0;
    volatile unsigned char *addr = start;
    while ((addr - start) < (int)_sizeof_bin)
    {
        writeBlock((volatile unsigned int *)addr, sector);
        addr += SECTORSIZE;
        sector++;
    }
}

void writeBlock(volatile unsigned int *memaddr, unsigned int sector)
{
    for (int i = 0; i < SECTORSIZE; i += 4)
    {
        SD_DATA_BASE[127 - (i >> 2)] = bin[(sector * SECTORSIZE + i) >> 2]; // 4byte offset
    }
    *SD_ADRS = sector;
    *SD_OP = 0x00000002;
    spiWait();
}
IMPORT_BIN(".rodata", "kernel.bin", bin);