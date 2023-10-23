RISCV_PREFIX=riscv32-unknown-elf-
AS=$(RISCV_PREFIX)as
LD=$(RISCV_PREFIX)ld
GCC=$(RISCV_PREFIX)gcc
OBJCOPY=$(RISCV_PREFIX)objcopy
TARGET=test
LSCRIPT=program
BOOTSTRAP=bootloader.c
MIFS=bootrom.hex $(TARGET).hex 
BOOT=/root/software/opensbi/build/platform/rv32xsoc/firmware/fw_payload.bin
BOOTSTRAP_OPT=-DBINSIZE=$(shell ls -lL $(BOOT) | awk '{print $$5}')

all: $(MIFS) 

bootrom.elf: $(BOOTSTRAP)
ifneq ($(filter %.s, $(BOOTSTRAP)),)
	$(AS) $< -o .bootrom.elf
	$(LD) .bootrom.elf -o $@ -T bootrom.ld
	@rm .bootrom.elf
else ifneq ($(filter %.c, $(BOOTSTRAP)),)
	$(GCC) set_sp.s $(BOOTSTRAP) $(BOOTSTRAP_OPT) -march=rv32ima_zicsr_zifencei -mabi=ilp32 -e _boot -nostdinc -nostdlib -fno-builtin -T bootrom.ld -o bootrom.elf
endif


$(TARGET).elf: $(TARGET).c
	make -C /root/software/helloworld TEST=$(shell pwd)/$(TARGET) $(shell pwd)/$(TARGET).elf

%.hex: %.elf
	$(OBJCOPY) -O verilog $< $@
	
clean:
	@rm -f *.elf *.hex
