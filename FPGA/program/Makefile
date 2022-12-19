RISCV_PREFIX=riscv32-unknown-elf-
AS=$(RISCV_PREFIX)as
LD=$(RISCV_PREFIX)ld
GCC=$(RISCV_PREFIX)gcc
VERSION=$(shell $(CC) -dumpversion)
OBJCOPY=$(RISCV_PREFIX)objcopy
TARGET=test
GLOSS=/root/software/gloss
LDFLAGS=-L$(GLOSS) -L/opt/riscv/riscv32-unknown-elf/lib -L/opt/riscv/lib/gcc/riscv32-unknown-elf/$(VERSION)/lib -lcrt0 -lc -lm -lgloss_riscvx -lgloss -lg -lgcov -lgcc -T$(LSCRIPT).ld
LSCRIPT=program
BOOTROM=bootrom.s
MIFS=bootrom.hex $(TARGET).hex 
BROPT=-DBINSIZE=25762824
OPT=
CFLAGS=$(OPT) -mabi=ilp32 -nostartfiles -nostdinc -nostdlib -static -fno-builtin-march=rv32im -mabi=ilp32 -I/opt/riscv/riscv32-unknown-elf/include -I/opt/riscv/lib/gcc/riscv32-unknown-elf/$(VERSION)/include

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
	$(GCC) $(TARGET).c $(CFLAGS) -c -o $(TARGET).o
	make all -C $(GLOSS)
	$(GCC) $(CFLAGS) $(TARGET).o -o $@ $(LDFLAGS)

#$(GCC) $(TARGET).c $(OPT) -mabi=ilp32 -e main -nostdinc -nostdlib -fno-builtin -T $(LSCRIPT).ld -o $(TARGET).elf

%.hex: %.elf
	$(OBJCOPY) -O verilog $< $@
	
clean:
	@rm -f *.elf *.hex
