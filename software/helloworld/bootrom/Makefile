RISCV_PREFIX=riscv32-unknown-elf-
AS=$(RISCV_PREFIX)as
LD=$(RISCV_PREFIX)ld
OBJCOPY=$(RISCV_PREFIX)objcopy

bootrom.elf: bootrom.s
	$(AS) $< -o .bootrom.elf
	$(LD) .bootrom.elf -o $@ -T link.ld
	@rm .bootrom.elf
bootrom.hex: bootrom.elf
	$(OBJCOPY) -O verilog $< $@
clean:
	@rm -f bootrom.hex bootrom.elf .bootrom.elf
