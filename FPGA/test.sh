#!/bin/bash -x

OBJCOPY=riscv32-unknown-elf-objcopy
SRCDIR=$1

$SRCDIR/hextomif.py $SRCDIR/program/bootrom.hex $SRCDIR
#$OBJCOPY -O verilog $2 $2.hex
$SRCDIR/hextomif.py $2.hex $SRCDIR