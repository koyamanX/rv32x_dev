#define READ 1'b0
#define WRITE 1'b1

declare cache_data interface {
    input address[8];
    input clock;
    input data[128];
    input wren;
    output q[128];
}