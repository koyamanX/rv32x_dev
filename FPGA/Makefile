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

VPATH		= $(SRCDIR)
WORKDIR		= synth
DESIGN		= rv32x
BOARD		= DE10_lite
NSL2VL    	= nsl2vl
NSLFLAGS  	= -O2 -neg_res -I$(SRCDIR)/../core \
-I$(SRCDIR)/../integration \
-I$(SRCDIR)/../integration/mmcspi/src \
-I$(SRCDIR)/bram \
-I$(SRCDIR)/pll \
-I$(SRCDIR)/SDRAM_Controler \
-DCV -D$(BOARD) -DCPU_FREQ=$(CPU_FREQ) -DPLL_DIV=$(PLL_DIV) -DPLL_MUL=$(PLL_MUL) -D$(RAM)
MKPROJ		= $(SRCDIR)/mkproj-$(BOARD).tcl
Q2SH		= $(QUARTUS_DIR)quartus_sh$(EXTENSION)
Q2PGM		= $(QUARTUS_DIR)quartus_pgm$(EXTENSION)
Q2CDB		= $(QUARTUS_DIR)quartus_cdb$(EXTENSION)
Q2ASM		= $(QUARTUS_DIR)quartus_asm$(EXTENSION)
CABLE		= "USB-Blaster [USB-0]"
PMODE		= JTAG
NSRC := $(wildcard $(SRCDIR)/../core/*.nsl) \
 $(wildcard $(SRCDIR)/../integration/*.nsl) \
 $(wildcard $(SRCDIR)/../integration/mmcspi/src/*.nsl) \
 $(wildcard $(SRCDIR)/*.nsl) \
 $(wildcard $(SRCDIR)/bram/*.nsl) \
 $(wildcard $(SRCDIR)/pll/*.nsl) \
 $(wildcard $(SRCDIR)/SDRAM_Controler/*.nsl)
#SRCS		:= $(wildcard $(SRCDIR)/*.nsl)
#VFILES 		= $(SRCS:$(SRCDIR)/%.nsl=%.v) 
VFILES := $(patsubst %.nsl,%.v,$(notdir $(NSRC)))
HEXS := bootrom.hex
LIBS		= 
RESULT		= result.txt
TARGET	= mifSrc/test
_TARGET = $(SRCDIR)/$(TARGET)
TEST = $(filter-out %.hex,$(wildcard $(SRCDIR)/tests/*))
TEST2 = $(filter-out %.hex, $(wildcard $(SRCDIR)/bram/*.nsl))
TOP = top_sdram	#top module
SDC = $(DESIGN).sdc
STP = $(DESIGN).stp
SERIAL_CABLE_LEVEL = TTL
ifeq ($(BOARD),DE0_CV)
	DEVICE_FAMILY = "Cyclone V"
else ifeq ($(BOARD),DE10_lite)
	DEVICE_FAMILY = "MAX 10"
endif
_DEVICE_FAMILY=$(subst ",\",$(DEVICE_FAMILY))
PLL_INFREQ=50000000
PLL_DIV=5
PLL_MUL=4
CPU_FREQ=$(shell echo "scale=1; $(PLL_INFREQ) / $(PLL_DIV) * $(PLL_MUL)" | bc | awk -F"." '{print $$1}')	#40.0MHz
RAM=SDRAM
########

all:
	-@if [ ! -d $(WORKDIR) ]; then \
		echo mkdir $(WORKDIR); \
		mkdir $(WORKDIR); \
	else \
		rm $(WORKDIR)/top.v $(WORKDIR)/pll.v; \
	fi
	sed -i -e "s/NUM_PARALLEL_PROCESSORS.*/NUM_PARALLEL_PROCESSORS $(shell nproc)/g" $(MKPROJ)
	sed -i -e "s/IO_STANDARD.*RXD/IO_STANDARD \"3.3-V LV$(SERIAL_CABLE_LEVEL)\" -to RXD/g" $(MKPROJ)
	sed -i -e "s/IO_STANDARD.*TXD/IO_STANDARD \"3.3-V LV$(SERIAL_CABLE_LEVEL)\" -to TXD/g" $(MKPROJ)
	( cd $(WORKDIR); make -f ../Makefile SRCDIR=.. compile )

########

.SUFFIXES: .v .nsl
.PHONY: riscv_tests test test2 mifs sdc pll

%.v: $(SRCDIR)/../integration/%.nsl 
	$(NSL2VL) $(NSLFLAGS)  $< -o $@
	
%.v: $(SRCDIR)/../integration/mmcspi/src/%.nsl 
	$(NSL2VL) $(NSLFLAGS)  $< -o $@

%.v: $(SRCDIR)/../core/%.nsl 
	$(NSL2VL) $(NSLFLAGS)  $< -o $@

%.v: $(SRCDIR)/%.nsl
	$(NSL2VL) $(NSLFLAGS)  $< -o $@

%.v: $(SRCDIR)/bram/%.nsl
	sed -i -e "s/intended_device_family = .*/intended_device_family = $(_DEVICE_FAMILY),/g" $< 
	$(NSL2VL) $(NSLFLAGS)  $< -o $@

%.v: $(SRCDIR)/pll/%.nsl
	$(NSL2VL) $(NSLFLAGS)  $< -o $@

%.v: $(SRCDIR)/SDRAM_Controler/%.nsl
	$(NSL2VL) $(NSLFLAGS)  $< -o $@
	
mifs:
	$(SRCDIR)/mifGen.sh $(SRCDIR) $(_TARGET)
#	ls *.mif | xargs -t -i mv {} $(SRCDIR)/synth/

update_mifs: mifs	#update main memory mifs without recompile
	cd $(WORKDIR); make -f ../Makefile _update_mifs

_update_mifs:
	$(Q2CDB) $(DESIGN) -c $(DESIGN) --update_mif
	$(Q2ASM) --read_settings_files=off --write_settings_files=off $(DESIGN) -c $(DESIGN)

sdc:
	cp $(SRCDIR)/$(SDC) .

# pll: 
# 	$(SRCDIR)/SDRAM_Controler/PLLgen.sh
# 	cp pll/pll.v ./
	
riscv-tests:
	make -C $(SRCDIR)/../software/riscv-tests/isa XLEN=32
	if [ ! -d $(SRCDIR)/tests ]; then \
		mkdir $(SRCDIR)/tests; \
	fi
	find $(SRCDIR)/../software/riscv-tests/isa -maxdepth 1 -perm -111 -type f | xargs -i cp -p {} $(SRCDIR)/tests
	ls $(TEST) | xargs -t -i riscv32-unknown-elf-objcopy -O verilog {} {}.hex

test:
	echo $(Q2SH)

test2:
	@echo $(CPU_FREQ)

$(DESIGN).qsf: $(VFILES) $(LIBS)
	$(Q2SH) -t $(MKPROJ) -project $(DESIGN) $^

$(DESIGN).sof: $(DESIGN).qsf
	$(Q2SH) --flow compile $(DESIGN)

########

compile: sdc mifs $(DESIGN).sof	#.sofでPL作成、.qsfでquartusのGUI形式生成のみできる
#	@echo "**** $(DESIGN).fit.summary" | tee -a $(RESULT)
#	@cat $(DESIGN).fit.summary | tee -a $(RESULT)
#	@echo "**** $(DESIGN).tan.rpt" | tee -a $(RESULT)
#	@grep "Info: Fmax" $(DESIGN).tan.rpt | tee -a $(RESULT)

download: config-n

config: all
	$(Q2PGM) -c $(CABLE) -m $(PMODE) -o "p;$(WORKDIR)/$(DESIGN).sof"
config-n: # without re-compile
	$(Q2PGM) -c $(CABLE) -m $(PMODE) -o "p;$(WORKDIR)/$(DESIGN).sof"

clean:
	rm -rf - $(WORKDIR)

########

#$(DESIGN).v	: $(DESIGN).nsl
