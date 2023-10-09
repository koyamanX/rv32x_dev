#define IMEM 2'b01
#define DMEM 2'b10
#define ICACHE 2'b01
#define DCACHE 2'b10
#define EMPTY 2'b00
#define IR 2'b01
#define DR 2'b10
#define DW 2'b11
#define READ 1'b1
#define WRITE 1'b0

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

    input DRAM_DI[16];
    output DRAM_DO[16];
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

    func_out write();
    input sdrctr_clk;
    input mmc_clk;
}