RISCV=/opt/riscv
RISCV_PREFIX=riscv32-unknown-elf
TOOL_VERSION=10.1.0
CC=$(RISCV_PREFIX)-gcc
AS=$(RISCV_PREFIX)-as
AR=$(RISCV_PREFIX)-ar
LD=$(RISCV_PREFIX)-ld
OBJCOPY=$(RISCV_PREFIX)-objcopy
OBJDUMP=$(RISCV_PREFIX)-objdump
READELF=$(RISCV_PREFIX)-readelf
SIZE=$(RISCV_PREFIX)-size
PAGER=less

MABI="-mabi=ilp32"
MARCH="-march=rv32ima"
LINKERSCRIPT=-Tlink.ld

LIBGLOSS_PATH=../gloss
SIMULATOR_PATH=../../simulation
SIMULATOR=rv32x_simulation
SIMULATOR_FLAGS=--no-sim-exit --no-log 2> /dev/null || true
BOOTROM=bootrom

NEWLIB_INCLUDE=-I$(RISCV)/$(RISCV_PREFIX)/include -I$(RISCV)/lib/gcc/$(RISCV_PREFIX)/$(TOOL_VERSION)/include
CFLAGS=-nostartfiles -nostdlib -nostdinc -static -O2 $(MABI) $(MARCH) $(NEWLIB_INCLUDE)
NEWLIB_LIBRARY_PATH=-L$(LIBGLOSS_PATH) -L$(RISCV)/$(RISCV_PREFIX)/lib -L$(RISCV)/lib/gcc/$(RISCV_PREFIX)/$(TOOL_VERSION)/lib
NEWLIB_LIBRARY=-lcrt0 -lc -lm -lgloss_riscvx -lgloss -lg -lgcov -lgcc $(LINKERSCRIPT)
LDFLAGS=$(NEWLIB_LIBRARY_PATH) $(NEWLIB_LIBRARY)

exe=main
main_srcs = main.c crt0.S
main_c_srcs = $(filter %.c, $(main_srcs))
main_c_objs = $(patsubst %.c, %.o, $(notdir $(main_c_srcs)))
main_asm_srcs = $(filter %.S, $(main_srcs))
main_asm_objs = $(patsubst %.S, %.o, $(notdir $(main_asm_srcs)))

.PHONY: clean all run dump size
.DEFAULT_GOAL=all
.SUFFIXES=.c .o .S

all: $(exe)

$(main_c_objs): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
objs += $(main_c_objs)

$(main_asm_objs): %.o : %.S
	$(CC) $(CFLAGS) -c $< -o $@
objs += $(main_asm_objs)

$(exe): $(objs)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(exe).hex: $(exe)
	$(OBJCOPY) -O verilog $< $@

dump: $(exe)
	$(OBJDUMP) -D $< | $(PAGER)

size: $(exe)
	$(SIZE) $<

$(BOOTROM).hex:
	make -C $(BOOTROM) $@
	cp $(BOOTROM)/$@ $@

$(SIMULATOR):
	make -C $(SIMULATOR_PATH)
	cp $(SIMULATOR_PATH)/$(SIMULATOR) $(SIMULATOR)

run: $(exe) $(BOOTROM).hex $(SIMULATOR)
	@clear && ./$(SIMULATOR) $< $(SIMULATOR_FLAGS)

clean:
	@rm -rf $(objs) $(exe)
	@make -C $(BOOTROM) clean
	@rm -rf $(BOOTROM).hex
	@rm -rf $(SIMULATOR)


