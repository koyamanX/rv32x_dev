#!/usr/bin/env python3

import sys
import os


def formatBig(input):
    array = []
    cnt = 0
    ptr = -1
    with open(input, 'r') as f:
        l = f.readlines()
        for i in [i for i in l if '@' not in i]:
            a = i.split()
            while (len(a) < 4):
                a.append('00')
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


def mifGen(input, outdir):
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
    with open(os.path.join(outdir, os.path.splitext(os.path.basename(input))[0] + ".mif"), 'w') as f:
        f.write(buf)


if __name__ == "__main__":
    input = sys.argv[1]
    srcdir = sys.argv[2]
    outdir = os.path.join(srcdir, "synth")
    mifGen(input, outdir)
