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
DESIGN		= rv32x_bram
BOARD		= DE0_CV
NSL2VL    	= nsl2vl
NSLFLAGS  	= -O2 -neg_res -I$(SRCDIR)/../core -I$(SRCDIR)/../integration -I$(SRCDIR)/../integration/mmcspi/src -DMIF
MKPROJ		= $(SRCDIR)/mkproj-$(BOARD).tcl
Q2SH		= quartus_sh
Q2PGM		= quartus_pgm
CABLE		= "USB-Blaster [USB-0]"
PMODE		= JTAG
NSRC := $(wildcard $(SRCDIR)/../core/*.nsl) $(wildcard $(SRCDIR)/../integration/*.nsl) $(wildcard $(SRCDIR)/../integration/mmcspi/src/*.nsl) $(wildcard $(SRCDIR)/*.nsl)
#SRCS		:= $(wildcard $(SRCDIR)/*.nsl)
#VFILES 		= $(SRCS:$(SRCDIR)/%.nsl=%.v) 
VFILES := $(patsubst %.nsl,%.v,$(notdir $(NSRC)))
MIFSRCS:= $(wildcard $(SRCDIR)/program/*.hex)
MIFS := $(patsubst %.hex,%.mif,$(notdir $(MIFSRCS)))
LIBS		= 
RESULT		= result.txt

########

all:
	@if [ ! -d $(WORKDIR) ]; then \
		echo mkdir $(WORKDIR); \
		mkdir $(WORKDIR); \
	fi
	( cd $(WORKDIR); make -f ../Makefile SRCDIR=.. compile )

########

.SUFFIXES: .v .nsl
.PHONY: test test2

%.v: $(SRCDIR)/../integration/%.nsl 
	$(NSL2VL) $(NSLFLAGS)  $< -o $@
	
%.v: $(SRCDIR)/../integration/mmcspi/src/%.nsl 
	$(NSL2VL) $(NSLFLAGS)  $< -o $@

%.v: $(SRCDIR)/../core/%.nsl 
	$(NSL2VL) $(NSLFLAGS)  $< -o $@

%.v: $(SRCDIR)/%.nsl
	$(NSL2VL) $(NSLFLAGS)  $< -o $@

%.mif: $(MIFSRCS)
	$(SRCDIR)/hextomif.py $(SRCDIR)/program/bootrom.hex bootrom.mif
	$(SRCDIR)/hextomif.py $(SRCDIR)/program/program.hex program.mif

test2:
	echo $(MIFSRCS) $(MIFS) $(VFILES)

$(DESIGN).qsf: $(VFILES) $(LIBS) $(MIFS)
	$(Q2SH) -t $(MKPROJ) -project $(DESIGN) $^

$(DESIGN).sof: $(DESIGN).qsf $(MIFS)
	$(Q2SH) --flow compile $(DESIGN)

########

compile: $(DESIGN).qsf	#.sofでPL作成、.qsfでquartusのGUI形式生成のみできる
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
