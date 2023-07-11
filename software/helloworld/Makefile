RISCV_PREFIX=riscv32-unknown-elf-
CC=$(RISCV_PREFIX)gcc
VERSION=$(shell $(CC) -dumpversion)
CFLAGS=-nostartfiles -nostdlib -nostdinc -march=rv32ima_zicsr_zifencei -mabi=ilp32 -I/opt/riscv/riscv32-unknown-elf/include -I/opt/riscv/lib/gcc/riscv32-unknown-elf/$(VERSION)/include -static
AS=$(RISCV_PREFIX)as
LD=$(RISCV_PREFIX)ld
GLOSS=../gloss
LDFLAGS=-L$(GLOSS) -L/opt/riscv/riscv32-unknown-elf/lib -L/opt/riscv/lib/gcc/riscv32-unknown-elf/$(VERSION)/lib -lcrt0 -lc -lm -lgloss_riscvx -lgloss -lg -lgcov -lgcc -Tlink.ld
OBJCOPY=$(RISCV_PREFIX)objcopy
TEST=helloworld
SIMDIR=../../simulation
SIM=rv32x_simulation
BOOTROMDIR=bootrom
BOOTROM=bootrom

run: $(TEST).elf $(BOOTROM).hex
	make -C $(SIMDIR) -B
	cp $(SIMDIR)/$(SIM) $(SIM)
	cp $(SIMDIR)/gtk.sh ./
	./$(SIM) $< --no-sim-exit --no-log 2> /dev/null || true
$(TEST).elf: $(TEST).c 
	$(CC) $(CFLAGS) $< -c -o $(TEST).o
	make all -C $(GLOSS)
	$(CC) $(CFLAGS) $(TEST).o -o $@ $(LDFLAGS)
$(TEST).hex: $(TEST).elf
	$(OBJCOPY) -O verilog $< $@
$(BOOTROM).hex:
	make -C $(BOOTROMDIR) $@
	cp $(BOOTROM)/$@ $@
clean:
	@rm -f $(TEST).hex $(TEST).elf .$(TEST).elf *.o *.log *.vcd $(BOOTROM).hex $(SIM)
	@make -C bootrom clean
