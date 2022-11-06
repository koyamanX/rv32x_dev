#define READ 1'b0
#define WRITE 1'b1

declare mainmem interface {
    param_str filename;
    input address[13];
    input clock;
    input data[8];
    input wren;
    output q[8];
}
