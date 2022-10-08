RISCV_PREFIX=riscv32-unknown-elf-
AS=$(RISCV_PREFIX)as
LD=$(RISCV_PREFIX)ld
GCC=$(RISCV_PREFIX)gcc
OBJCOPY=$(RISCV_PREFIX)objcopy
MIFS=bootrom.hex program.hex

all: $(MIFS)

bootrom.elf: bootrom.s
	$(AS) $< -o .bootrom.elf
	$(LD) .bootrom.elf -o $@ -T bootrom.ld
	@rm .bootrom.elf

program.elf: test.c
	$(GCC) test.c -mabi=ilp32 -e main -nostdinc -nostdlib -fno-builtin -T program.ld -o program.elf

%.hex: %.elf
	$(OBJCOPY) -O verilog $< $@
	
clean:
	@rm -f bootrom.hex bootrom.elf program.hex program.elf
