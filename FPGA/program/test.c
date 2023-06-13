#include <stdio.h>

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

extern void _boot();
void spiWait();
void exeCMD(unsigned, unsigned, unsigned);
void printResult(unsigned, unsigned);
void printData(unsigned);
void printResp(unsigned);
void printCD();

void main(void)
{
    spiWait();
    exeCMD(58, 0, MMC_RSP_SPI_R3 | MMC_CMD_AC);
    exeCMD(9, 0, MMC_RECIEVE_BYTES(16) | MMC_RSP_SPI_R1 | MMC_CMD_ADTC | MMC_READ);
    exeCMD(10, 0, MMC_RECIEVE_BYTES(16) | MMC_RSP_SPI_R1 | MMC_CMD_ADTC | MMC_READ);
    exeCMD(55, 0, MMC_RSP_SPI_R1 | MMC_CMD_AC);
    exeCMD(51, 0, MMC_RECIEVE_BYTES(8) | MMC_RSP_SPI_R1 | MMC_CMD_ADTC | MMC_READ);
    exeCMD(55, 0, MMC_RSP_SPI_R1 | MMC_CMD_AC);
    exeCMD(13, 0, MMC_RECIEVE_BYTES(64) | MMC_RSP_SPI_R2 | MMC_CMD_ADTC | MMC_READ);
    exeCMD(13, 0, MMC_RSP_SPI_R2 | MMC_CMD_AC);
    exeCMD(17, 0x200, MMC_RECIEVE_BYTES(512) | MMC_RSP_SPI_R1 | MMC_CMD_ADTC | MMC_READ);

    /*
    exeCMD(32, 0, MMC_RSP_SPI_R1 | MMC_CMD_AC);
    exeCMD(33, 0x400, MMC_RSP_SPI_R1 | MMC_CMD_AC);
    exeCMD(34, 0x200, MMC_RSP_SPI_R1 | MMC_CMD_AC);
    exeCMD(38, 0, MMC_RSP_SPI_R1 | MMC_CMD_AC);
    */

    /*
    exeCMD(35, 0, MMC_RSP_SPI_R1 | MMC_CMD_AC);
    exeCMD(36, 0x10000, MMC_RSP_SPI_R1 | MMC_CMD_AC);
    exeCMD(37, 0, MMC_RSP_SPI_R1 | MMC_CMD_AC);
    exeCMD(38, 0, MMC_RSP_SPI_R1 | MMC_CMD_AC);
    */
    while (1)
        ;
}

void spiWait()
{
    while ((*MMC_STATUS) != 0x3)
    {
        asm volatile("nop");
    }
}

void exeCMD(unsigned cmd, unsigned arg, unsigned cmd_types)
{
    spiWait();
    *MMC_CMD = cmd;
    *MMC_ARG = arg;
    *MMC_CMD_TYPE = cmd_types;
    *MMC_STATUS = *MMC_STATUS | EXEC;
    printf("CMD%u, arg=0x%08x\n", cmd, arg);
    spiWait();
    printResult(cmd, cmd_types);
}

void printResult(unsigned cmd, unsigned cmd_types)
{
    printResp(cmd_types & MMC_RSP_SPI_MASK);
    if (cmd_types & MMC_CMD_ADTC)
    {
        printData((cmd_types & MMC_RECIEVE_BYTES_MASK) >> 16);
    }
}

void printData(unsigned len)
{
    printf("Data length: %u\n", len);
    printf("Data:\n");
    for (int i = 0; i < len; i++)
    {
        printf("%02x", *((unsigned char *)(MMC_DATA_BASE) + i));
        if ((i + 1) % 4 == 0)
        {
            printf(" ");
        }
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

void printResp(unsigned resType)
{
    switch (resType)
    {
    case MMC_RSP_SPI_R1: // and R1b
        printf("resp type: R1\n");
        printf("resp: %02x\n", *MMC_R1STAT);
        break;
    case MMC_RSP_SPI_R2:
        printf("resp type: R2\n");
        printf("resp: %02x %02x\n", *MMC_R1STAT, *MMC_RESP);
        break;
    case MMC_RSP_SPI_R3:
        printf("resp type: R3\n");
        printf("resp: %02x %08x\n", *MMC_R1STAT,
               *MMC_RESP);
        break;
    }
    printf("\n");
}