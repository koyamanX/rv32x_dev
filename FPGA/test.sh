OBJCOPY=riscv32-unknown-elf-objcopy
SRCDIR=$1

$SRCDIR/hextomif.py $SRCDIR/program/bootrom.hex
#$OBJCOPY -O verilog $2 $2.hex
$SRCDIR/hextomif.py $2.hex