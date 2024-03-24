
declare xpll interface
{
    input areset;
    input inclk0;
    output c0;
    output c1;
    output c2;
    output c3;
    output locked;
}

declare pll
{
    input reset;
    input refclk;
    output sdrctr_clk;
    output sdram_clk;
    output mmc_clk;
    output cpu_clk;
    output locked;
}