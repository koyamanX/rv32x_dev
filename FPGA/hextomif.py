#!/usr/bin/env python3

import sys
import os


def format(input):
    array = []
    with open(input, 'r') as f:
        l = f.readlines()
        for i in [i for i in l if '@' not in i]:
            a = i.split()
            for j in a:
                array.append(j)
        # print(array)
    return array


def formatBig(input):
    array = []
    cnt = 0
    ptr = -1
    with open(input, 'r') as f:
        l = f.readlines()
        for i in [i for i in l if '@' not in i]:
            a = i.split()
            for j in a:
                if cnt % 4 != 0:
                    array.insert(ptr, j)
                    ptr -= 1
                else:
                    array.append(j)
                    ptr = -1
                cnt += 1
        # print(array)
    return array


def memGen(input, outdir):
    addr = 0x0
    mem = []
    array = format(input)
    for num in range(4):
        mem.append(array[num::4])

    for num in range(4):
        addr = 0
        depth = 8192
        buf = f'DEPTH={depth};\nWIDTH=8;\nADDRESS_RADIX=HEX;\nDATA_RADIX=HEX;\nCONTENT BEGIN\n\n{0:04x} :'
        for i in mem[num]:
            if (addr != 0) and ((addr % 16 == 0)):
                buf += f';\n{addr:04x} :'
            buf += f' {i}'
            addr += 1
        if addr != depth:
            for i in range(depth-addr):
                if (addr != 0) and ((addr % 16 == 0)):
                    buf += f';\n{addr:04x} :'
                buf += f' 00'
                addr += 1
        buf += ';\nEND;'
        # print(buf)
        buf = buf.replace('@', str(addr))
        with open(os.path.join(outdir, "memory_"+str(num)+".mif"), 'w') as f:
            f.write(buf)


def bootromGen(input, outdir):
    addr = 0x0
    array = formatBig(input)  # convert to big endian
    cnt = 0
    buf = f'DEPTH=@;\nWIDTH=32;\nADDRESS_RADIX=HEX;\nDATA_RADIX=HEX;\nCONTENT BEGIN\n\n{0:04x} :'
    for i in array:
        if (addr != 0) and ((addr % 4 == 0) and cnt % 4 == 0):
            buf += f';\n{addr:04x} :'

        if ((cnt % 4) == 0):
            buf += f' {i}'
            addr += 1
        else:
            buf += f'{i}'
        cnt += 1
    buf += ';\nEND;'
    # print(buf)
    buf = buf.replace('@', str(addr))
    with open(os.path.join(outdir, "bootrom.mif"), 'w') as f:
        f.write(buf)


if __name__ == "__main__":
    input = sys.argv[1]
    srcdir = sys.argv[2]
    outdir = os.path.join(srcdir, "synth")
    if os.path.basename(input) == "bootrom.hex":
        bootromGen(input, outdir)
    else:
        memGen(input, outdir)
