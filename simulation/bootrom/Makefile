RISCV_PREFIX=riscv32-unknown-elf-
AS=$(RISCV_PREFIX)as
LD=$(RISCV_PREFIX)ld
GCC=$(RISCV_PREFIX)gcc
OBJCOPY=$(RISCV_PREFIX)objcopy
LOADEROPT=-DBINSIZE\=0	#-DHEAD=で開始アドレス -DBINSIZE=でロードするバイナリのサイズ、０だとロードルーチンを実行しない -DBINSTARTでバイナリの置いてあるセクタを指定

all: bootrom.hex

bootrom.elf: bootrom.s bootloader.c
	$(GCC) bootrom.s bootloader.c $(LOADEROPT) -mabi=ilp32 -march=rv32ima_zicsr_zifencei -e _boot -nostdinc -nostdlib -fno-builtin -T link.ld -o bootrom.elf

bootrom.hex: bootrom.elf
	$(OBJCOPY) -O verilog $< $@
clean:
	@rm -f bootrom.hex bootrom.elf .bootrom.elf bootloader.s
