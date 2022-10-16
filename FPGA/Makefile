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
BOARD		= DE0_CV
NSL2VL    	= nsl2vl
NSLFLAGS  	= -O2 -neg_res -I$(SRCDIR)/../core -I$(SRCDIR)/../integration -I$(SRCDIR)/../integration/mmcspi/src -I$(SRCDIR)/mem -DRISCV_TESTS -DCV
MKPROJ		= $(SRCDIR)/mkproj-$(BOARD).tcl
Q2SH		= quartus_sh
Q2PGM		= quartus_pgm
CABLE		= "USB-Blaster [USB-0]"
PMODE		= JTAG
NSRC := $(wildcard $(SRCDIR)/../core/*.nsl) $(wildcard $(SRCDIR)/../integration/*.nsl) $(wildcard $(SRCDIR)/../integration/mmcspi/src/*.nsl) $(wildcard $(SRCDIR)/*.nsl) $(wildcard $(SRCDIR)/mem/*.nsl)
#SRCS		:= $(wildcard $(SRCDIR)/*.nsl)
#VFILES 		= $(SRCS:$(SRCDIR)/%.nsl=%.v) 
VFILES := $(patsubst %.nsl,%.v,$(notdir $(NSRC)))
HEXS := bootrom.hex
LIBS		= 
RESULT		= result.txt
TARGET	= $(SRCDIR)/tests/rv32mi-p-breakpoint

########

all:
	@if [ ! -d $(WORKDIR) ]; then \
		echo mkdir $(WORKDIR); \
		mkdir $(WORKDIR); \
	fi
	( cd $(WORKDIR); make -f ../Makefile SRCDIR=.. compile )

########

.SUFFIXES: .v .nsl
.PHONY: riscv_tests test2 mifs

%.v: $(SRCDIR)/../integration/%.nsl 
	$(NSL2VL) $(NSLFLAGS)  $< -o $@
	
%.v: $(SRCDIR)/../integration/mmcspi/src/%.nsl 
	$(NSL2VL) $(NSLFLAGS)  $< -o $@

%.v: $(SRCDIR)/../core/%.nsl 
	$(NSL2VL) $(NSLFLAGS)  $< -o $@

%.v: $(SRCDIR)/%.nsl
	$(NSL2VL) $(NSLFLAGS)  $< -o $@

mem.v: $(SRCDIR)/mem/mem.nsl
	$(NSL2VL) $(NSLFLAGS)  $< -o $@
	sed -i -e "s/\"DEADBEEF\"/filename/g" mem.v

bram_ctr.v: $(SRCDIR)/mem/bram_ctr.nsl
	$(NSL2VL) $(NSLFLAGS)  $< -o $@
	sed -i -e "97i \ \ defparam memory.filename = \"memory_0.mif\";" bram_ctr.v
	sed -i -e "97i \ \ defparam memory_1.filename = \"memory_1.mif\";" bram_ctr.v
	sed -i -e "97i \ \ defparam memory_2.filename = \"memory_2.mif\";" bram_ctr.v
	sed -i -e "97i \ \ defparam memory_3.filename = \"memory_3.mif\";" bram_ctr.v

mifs:
	$(SRCDIR)/test.sh $(SRCDIR) $(TARGET)

riscv-tests:
	make -C $(SRCDIR)/../software/riscv-tests/isa XLEN=32
	if [ ! -d $(SRCDIR)/tests ]; then \
		mkdir $(SRCDIR)/tests; \
	fi
	find $(SRCDIR)/../software/riscv-tests/isa -maxdepth 1 -perm -111 -type f | xargs -i cp -p {} $(SRCDIR)/tests


test2:
	echo $(MIFSRCS) $(VFILES) $(HEXS)

$(DESIGN).qsf: $(VFILES) $(LIBS)
	$(Q2SH) -t $(MKPROJ) -project $(DESIGN) $^

$(DESIGN).sof: $(DESIGN).qsf
	$(Q2SH) --flow compile $(DESIGN)

########

compile: mifs $(DESIGN).sof	#.sofでPL作成、.qsfでquartusのGUI形式生成のみできる
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
