################################################################################
# Makefile : sof file generation using Quartus II
# Usage:
#		make compile for synthesis all files
#       make download for download .sof file to FPGA board
################################################################################
# 2011-09-12,13 Initial version by Y.Okuyama (based on PICO's makefile)
# 2012-10-08    CABLE is modified for linux environment
# 2013-07-09    Modified for DE0/windows environment
################################################################################

ifndef SRCDIR
SRCDIR	= .
endif

NSL2VL    	= nsl2vl
NSLFLAGS  	= -O2 -neg_res -I$(SRCDIR) \
-I$(SRCDIR)/../../core \
-I$(SRCDIR)/../../integration \
-I$(SRCDIR)/../../integration/mmcspi/src \
-I$(SRCDIR)/../SDRAM_Controler
NSRC := $(wildcard $(SRCDIR)/../../core/*.nsl) \
 $(wildcard $(SRCDIR)/../../integration/*.nsl) \
 $(wildcard $(SRCDIR)/../../integration/mmcspi/src/*.nsl) \
 $(wildcard $(SRCDIR)/*.nsl) \
 $(wildcard $(SRCDIR)/../SDRAM_Controler/*.nsl)
SIMVFILES 		= $(addprefix out/, $(patsubst %.nsl,%.v,$(notdir $(NSRC))))
SIMTOP = topsd_sim
TESTBENCH=rv32x_simulation

########

all: sim

########

.SUFFIXES: .v .nsl
.PHONY: test


sim: $(SIMVFILES)
	cp $(SRCDIR)/sdr.v out/
	verilator -Wno-UNSIGNED -Wno-STMTDLY -Wno-TIMESCALEMOD -Wno-REALCVT -Wno-INFINITELOOP -Wno-IMPLICIT -Wno-WIDTH -cc --trace --trace-underscore out/*.v -Isimulation --top-module $(SIMTOP) -exe $(TESTBENCH).cpp -O3
	make -C $(SRCDIR)/obj_dir/ -f V$(SIMTOP).mk
	cp $(SRCDIR)/obj_dir/V$(SIMTOP) $(SRCDIR)/rv32x_simulation
	make -C ../program BOOTROM=bootloader.c -B BROPT="-DBINSIZE=18340"
	cp ../program/bootrom.hex .

out/%.v: $(NSRC)
	if [ ! -d out ]; then \
		mkdir out; \
	fi
	$(NSL2VL) $(NSLFLAGS) $(filter $(shell echo $^ | grep "[^ ]*$*.nsl" -o), $^) -o $@
	
test:
	@echo $(NSRC)

clean:
	@rm -rf out/ obj_dir/