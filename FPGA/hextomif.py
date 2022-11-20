#!/usr/bin/env python3

import sys
import os


def formatBig(input):
    array = []
    cnt = 0
    ptr = -1
    paddinglen = 0
    with open(input, 'r') as f:
        l = f.readlines()
        for i in l:  # [i for i in l if '@' not in i]:
            a = i.split()
            if '@' in i:
                sec_end_addr = int(i.replace('@', ''), 16)
                a = []
                if(cnt != 0):
                    i = 0
                    paddinglen = (sec_end_addr - sec_start_addr) - \
                        (cnt - sec_start_cnt)
                    while (i < paddinglen):
                        a.append('00')
                        i += 1
                sec_start_cnt = cnt + paddinglen
                sec_start_addr = sec_end_addr
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
