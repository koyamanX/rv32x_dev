#define READ 1'b0
#define WRITE 1'b1

declare cache_data interface {
    input address[8];
    input clock;
    input data[128];
    input wren;
    output q[128];
}

declare cache_tag interface {
    input address[8];
    input clock;
    input data[20];
    input wren;
    output q[20];
}

declare mainmem interface {
    param_str filename;
    input address[15];
    input clock;
    input data[32];
    input wren;
    output q[32];
}

declare rom_1p interface {
    input address[10];
    input clock;
    output q[32];
}