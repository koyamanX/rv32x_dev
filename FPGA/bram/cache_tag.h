#define READ 1'b0
#define WRITE 1'b1

declare cache_tag interface {
    input address[8];
    input clock;
    input data[20];
    input wren;
    output q[20];
}
