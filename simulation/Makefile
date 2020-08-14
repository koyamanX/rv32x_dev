TOP=rv32x_simulation
NSL2VL=nsl2vl
NFLAGS=-O2 -neg_res -I../core -I../integration -DDEBUG
#SIMFLAGS=-verisim2 -target $(TOP)
INCOPT=-I../core
NSRC := $(wildcard ../core/*.nsl) $(wildcard ../integration/*.nsl) $(wildcard ../simulation/*.nsl)
SRC := $(notdir $(NSRC)) 
# VFILES := $(addprefix out/, $(SRC:.nsl=.v))
VFILES := $(addprefix out/, $(SRC:.nsl=.v)) 
RISCV_PREFIX=riscv32-unknown-elf-
AS=$(RISCV_PREFIX)as
LD=$(RISCV_PREFIX)ld
OBJCOPY=$(RISCV_PREFIX)objcopy
RISCV_INSTALL_DIR=/opt/riscv/x86_64-pc-linux-gnu/riscv32-elflittle

.PHONY: clean all obj_dir $(TOP)
.SUFFIXES: .nsl .v

all:$(TOP)
lint: $(VFILES)
	verilator --lint-only -Wall -Wno-UNUSED -Wno-UNDRIVEN --Wno-VARHIDDEN rv32x_simulation -Iout/
	
run_riscv_tests: $(TOP)
	make -C ../software/riscv-tests/isa XLEN=32
	./run_riscv_tests.py
out/%.v: ../integration/%.nsl 
	if [ ! -d out ]; then \
		mkdir out; \
	fi
	$(NSL2VL) $(NFLAGS) $< -o $@

out/%.v: ../core/%.nsl 
	if [ ! -d out ]; then \
		mkdir out; \
	fi
	$(NSL2VL) $(NFLAGS) $< -o $@
out/$(TOP).v: $(TOP).nsl 
	if [ ! -d out ]; then \
		mkdir out; \
	fi
	$(NSL2VL) $< $(NFLAGS) $(SIMFLAGS) -o $@
obj_dir: $(TOP).cpp $(VFILES) 
	make out/$(TOP).v SIMFLAGS=
	make -C elfloader libelfloader.a
	verilator --cc -Wno-lint -Wno-STMTDLY -Wno-INFINITELOOP -Wno-UNOPTFLAT --trace --trace-underscore out/*.v --top-module $(TOP) --exe $(TOP).cpp -O3 --autoflush -LDFLAGS "-L../elfloader -L$(RISCV_INSTALL_DIR)/lib -lelfloader -lopcodes -lbfd -liberty -lz -ldl" -CFLAGS "-I$(RISCV_INSTALL_DIR)/include -O2 -static"

$(TOP): $(TOP).cpp obj_dir $(VFILES) bootrom.hex
	cd obj_dir && make -f V$(TOP).mk V$(TOP)
	cp obj_dir/V$(TOP) $(TOP)
bootrom.hex: 
	make -C bootrom bootrom.hex
	cp bootrom/bootrom.hex ./
clean:
	@rm -rf out 
	@rm -rf $(TOP).v 
	@rm -rf obj_dir $(TOP) $(TOP).vcd
	@rm -rf failed
	@rm -rf bootrom.hex
	@rm -rf logs
	make clean -C elfloader 
	make clean -C bootrom
