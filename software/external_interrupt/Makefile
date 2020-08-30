RISCV_PREFIX=riscv32-unknown-elf-
CC=$(RISCV_PREFIX)gcc
CFLAGS=-nostartfiles -nostdlib -nostdinc -march=rv32im -mabi=ilp32
AS=$(RISCV_PREFIX)as
LD=$(RISCV_PREFIX)ld
OBJCOPY=$(RISCV_PREFIX)objcopy
TEST=helloworld
STARTUP=start
SIMDIR=../../simulation
SIM=rv32x_simulation
BOOTROMDIR=bootrom
BOOTROM=bootrom

run: $(TEST).elf $(BOOTROM).hex
	make -C $(SIMDIR)
	cp $(SIMDIR)/$(SIM) $(SIM)
	clear
	#./$(SIM) $< --print-inst-trace --print-disasm --print-exception 2> /dev/null || true
	./$(SIM) $< 2> /dev/null || true
$(TEST).elf: $(TEST).c $(STARTUP).s
	$(CC) $(CFLAGS) $< -c -o $(TEST).o
	$(CC) $(CFLAGS) $(STARTUP).s -c -o $(STARTUP).o
	$(LD) $(STARTUP).o $(TEST).o -T link.ld -o $@
$(TEST).hex: $(TEST).elf
	$(OBJCOPY) -O verilog $< $@
$(BOOTROM).hex:
	make -C $(BOOTROMDIR) $@
	cp $(BOOTROM)/$@ $@
clean:
	@rm -f $(TEST).hex $(TEST).elf .$(TEST).elf *.o *.log *.vcd $(BOOTROM).hex $(SIM)
	@make -C bootrom clean
