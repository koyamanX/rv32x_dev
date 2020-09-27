RISCV_PREFIX=riscv32-unknown-elf-
CC=$(RISCV_PREFIX)gcc
CFLAGS=-nostartfiles -nostdlib -nostdinc -march=rv32im -mabi=ilp32 -I/opt/riscv/riscv32-unknown-elf/include -I/opt/riscv/lib/gcc/riscv32-unknown-elf/10.1.0/include -static
AS=$(RISCV_PREFIX)as
LD=$(RISCV_PREFIX)ld
GLOSS=../gloss
LDFLAGS=-L$(GLOSS) -L/opt/riscv/riscv32-unknown-elf/lib -L/opt/riscv/lib/gcc/riscv32-unknown-elf/10.1.0/lib -lcrt0 -lc -lm -lgloss_riscvx -lgloss -lg -lgcov -lgcc -Tlink.ld
OBJCOPY=$(RISCV_PREFIX)objcopy
TEST=helloworld
SIMDIR=../../simulation
SIM=rv32x_simulation
BOOTROMDIR=bootrom
BOOTROM=bootrom

run: $(TEST).elf $(BOOTROM).hex
	make rv32x_simulation -C $(SIMDIR)
	cp $(SIMDIR)/$(SIM) $(SIM)
	clear
	./$(SIM) $< --no-log 2> /dev/null || true
	vcd2fst rv32x_simulation.vcd rv32x_simulation.fst
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
	@rm -f $(TEST).hex $(TEST).elf .$(TEST).elf *.o *.log *.vcd $(BOOTROM).hex $(SIM) *.fst
	@make -C bootrom clean
