#!/usr/bin/env python3

import sys


def hextomif(input):
    array = []
    addr = 0x0
    mem = []
    with open(input, 'r') as f:
        l = f.readlines()
        for i in [i for i in l if '@' not in i]:
            a = i.split()
            for j in a:
                array.append(j)
        # print(array)
    for num in range(4):
        mem.append(array[num::4])

    for num in range(4):
        addr = 0
        buf = f'DEPTH=@;\nWIDTH=8;\nADDRESS_RADIX=HEX;\nDATA_RADIX=HEX;\nCONTENT BEGIN\n\n{0:04x} :'
        for i in mem[num]:
            if (addr != 0) and ((addr % 16 == 0)):
                buf += f';\n{addr:04x} :'
            buf += f' {i}'
            addr += 1
        buf += ';\nEND;'
        # print(buf)
        buf = buf.replace('@', str(addr))
        with open("memory_"+str(num)+".mif", 'w') as f:
            f.write(buf)


if __name__ == "__main__":
    input = sys.argv[1]
    hextomif(input)
