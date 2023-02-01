#include <stdio.h>

#define MMC_BASE 0x40001000
#define MMC_STATUS ((volatile unsigned int *)(MMC_BASE + 0x200))
#define MMC_R1STAT ((volatile unsigned int *)(MMC_BASE + 0x204))
#define MMC_RESP ((volatile unsigned int *)(MMC_BASE + 0x208))
#define MMC_CMD ((volatile unsigned int *)(MMC_BASE + 0x20C))
#define MMC_ARG ((volatile unsigned int *)(MMC_BASE + 0x210))
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

extern void _boot();
void spiWait();
void exeCMD(unsigned, unsigned);
void printResult(unsigned);
void printData(unsigned);
void printResp(unsigned);
void printCD();

void main(void)
{
    spiWait();
    exeCMD(58, 0);
    exeCMD(9, 0);
    exeCMD(10, 0);
    exeCMD(55, 0);
    exeCMD(51, 0);
    exeCMD(13, 0);
    exeCMD(17, 512);
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

void exeCMD(unsigned cmd, unsigned arg)
{
    spiWait();
    *MMC_CMD = cmd;
    *MMC_ARG = arg;
    printf("CMD%u, arg=0x%08x\n", *MMC_CMD, *MMC_ARG);
    *MMC_STATUS = *MMC_STATUS | EXEC;
    spiWait();
    printResult(cmd);
}

void printResult(unsigned cmd)
{
    switch (cmd)
    {
    case 9:
        printResp(R1);
        printf("CSD:\n");
        printCD();
        break;
    case 10:
        printResp(R1);
        printf("CID:\n");
        printCD();
        break;
    case 13:
        printResp(R2);
        break;
    case 17:
        printResp(R1);
        printData(512);
        break;
    // 18,24,25 is TODO
    case 51:
        printResp(R1);
        printf("SCR:\n");
        printCD();
        break;
    case 58:
        printResp(R3);
        break;
    default:
        printResp(R1);
        break;
    }
}

void printData(unsigned len)
{
    printf("Data length: %u\n", len);
    printf("Data:\n");
    for (int i = 0; i < (len / 4); i++)
    {
        printf("%08x ", *(MMC_DATA_BASE + i));
        if ((i + 1) % 4 == 0)
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
    case R1: // and R1b
        printf("resp type: R1\n");
        printf("resp: %02x\n", *MMC_R1STAT);
        break;
    case R2:
        printf("resp type: R2\n");
        printf("resp: %02x %02x\n", *MMC_R1STAT, *MMC_RESP);
        break;
    case R3:
        printf("resp type: R3\n");
        printf("resp: %02x %08x\n", *MMC_R1STAT,
               *MMC_RESP);
        break;
    }
    printf("\n");
}

void printCD()
{
    unsigned int buf[4] = {*(MMC_DATA_BASE), *(MMC_DATA_BASE + 1), *(MMC_DATA_BASE + 2), *(MMC_DATA_BASE + 3)};
    unsigned int conv;
    for (int i = 0; i < 4; i++)
    {
        conv = 0;
        conv |= buf[i] >> 24 & 0x000000FF;
        conv |= buf[i] >> 8 & 0x0000FF00;
        conv |= buf[i] << 8 & 0x00FF0000;
        conv |= buf[i] << 24 & 0xFF000000;
        printf("%08x ", conv);
    }
    printf("\n");
}