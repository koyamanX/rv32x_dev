RISCV_PREFIX=riscv32-unknown-elf-
AS=$(RISCV_PREFIX)as
LD=$(RISCV_PREFIX)ld
GCC=$(RISCV_PREFIX)gcc
OBJCOPY=$(RISCV_PREFIX)objcopy
TARGET=test
LSCRIPT=program
BOOTROM=bootrom.s
MIFS=bootrom.hex $(TARGET).hex 
BROPT=-DBINSIZE=25762824
OPT=
USEGLOSS=	#if 1 use glosslib(like stdio)

all: $(MIFS) 

bootrom.elf: $(BOOTROM)
ifneq ($(filter %.s, $(BOOTROM)),)
	$(AS) $< -o .bootrom.elf
	$(LD) .bootrom.elf -o $@ -T bootrom.ld
	@rm .bootrom.elf
else ifneq ($(filter %.c, $(BOOTROM)),)
	$(GCC) set_sp.s $(BOOTROM) $(BROPT) -mabi=ilp32 -e _boot -nostdinc -nostdlib -fno-builtin -T bootrom.ld -o bootrom.elf
endif


$(TARGET).elf: $(TARGET).c
ifneq ($(USEGLOSS),)
	make -C /root/software/helloworld TEST=$(shell pwd)/$(TARGET) $(shell pwd)/$(TARGET).elf
else
	$(GCC) $(TARGET).c $(OPT) -mabi=ilp32 -e main -nostdinc -nostdlib -fno-builtin -T $(LSCRIPT).ld -o $(TARGET).elf
endif

%.hex: %.elf
	$(OBJCOPY) -O verilog $< $@
	
clean:
	@rm -f *.elf *.hex
