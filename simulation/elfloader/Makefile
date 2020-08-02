CC=gcc
AR=ar
RANLIB=ranlib
RISCV_INSTALL_DIR=/opt/riscv/x86_64-pc-linux-gnu/riscv32-elflittle
INCLUDE=$(RISCV_INSTALL_DIR)/include

all: libelfloader.a
libelfloader.a: elfloader.c elfloader.h
# Include bfd headers for RISCV
	$(CC) -c $< -I$(INCLUDE)
	$(AR) rcs $@ elfloader.o 
	$(RANLIB) $@
clean:
	@rm -f *.o *.a
