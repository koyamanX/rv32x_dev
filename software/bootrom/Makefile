DTS=rv32xsoc.dts
DTB=rv32xsoc.dtb
BOOTROM=bootrom
DTC=dtc
RISCV_PREFIX=riscv32-unknown-elf-
AS=$(RISCV_PREFIX)as
LD=$(RISCV_PREFIX)ld
OBJCOPY=$(RISCV_PREFIX)objcopy
OBJCOPY_OPTS=--add-section=dtb=$(DTB) --set-section-flags=dtb=contents,alloc,load,readonly,data --set-section-alignment=dtb=4 --change-section-address=dtb=0x100

$(BOOTROM).elf: $(BOOTROM).S rv32xsoc.dtb
	$(AS) $< -o .$(BOOTROM).elf
	$(LD) .$(BOOTROM).elf -o $@ -T link.ld
	@rm .$(BOOTROM).elf
$(BOOTROM).hex: $(BOOTROM).elf
	$(OBJCOPY) $(OBJCOPY_OPTS) -O verilog $< $@
$(DTB): $(DTS)
	$(DTC) -I dts -O dtb -o $@ $<
clean:
	@rm -f bootrom.hex bootrom.elf .bootrom.elf rv32xsoc.dtb
