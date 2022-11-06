
declare top_sdram
{
    output SCLK;
    input MISO;
    output MOSI;
    output CS;
    input RXD;
    output TXD;
    output LEDR[10];
    output HEX0[7];
    output HEX1[7];
    output HEX2[7];
    output HEX3[7];
    output HEX4[7];
    output HEX5[7];
    input SW[10];

    inout DRAM_DQ[16];
    output DRAM_ADDR[13];
    output DRAM_BA[2];
    output DRAM_LDQM;
    output DRAM_UDQM;
    output DRAM_RAS_N;
    output DRAM_CAS_N;
    output DRAM_CKE;
    output DRAM_CLK;
    output DRAM_WE_N;
    output DRAM_CS_N;
}