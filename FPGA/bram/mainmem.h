#define READ 1'b0
#define WRITE 1'b1

declare mainmem interface {
    input address[14];
    input clock;
    input data[32];
    input wren;
    output q[32];
}
