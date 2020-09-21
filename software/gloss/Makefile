RISCV_PREFIX=riscv32-unknown-elf-
CC=$(RISCV_PREFIX)gcc
CFLAGS=-nostartfiles -nostdlib -nostdinc -march=rv32im -mabi=ilp32 -I/opt/riscv/riscv32-unknown-elf/include -I/opt/riscv/lib/gcc/riscv32-unknown-elf/10.1.0/include -static
AS=$(RISCV_PREFIX)as
AR=$(RISCV_PREFIX)ar
RANLIB=$(RISCV_PREFIX)ranlib
LD=$(RISCV_PREFIX)ld
# Used for link executable 
LDFLAGS=-L. -L/opt/riscv/riscv32-unknown-elf/lib -lcrt0 -lc -lgloss_riscvx -lgloss -lg -Tlink.ld
OBJCOPY=$(RISCV_PREFIX)objcopy


gloss_srcs = \
	sbrk.c \
	nanosleep.c \
	times.c \
	uart.c 

crt0_srcs = crt0.S \
			trap_handler.S \
			isr.c \
			syscalls.c 
all_dummy: all

gloss_c_srcs = $(filter %.c, $(gloss_srcs))
gloss_c_objs = $(patsubst %.c, %.o, $(notdir $(gloss_c_srcs)))

$(gloss_c_objs): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
gloss_objs += $(gloss_c_objs)

gloss_lib = libgloss_riscvx.a
$(gloss_lib): $(gloss_objs)
	$(AR) rcv $@ $^
	$(RANLIB) $@

crt0_asm_srcs = $(filter %.S, $(crt0_srcs))
crt0_asm_objs = $(patsubst %.S, %.o, $(notdir $(crt0_asm_srcs)))
$(crt0_asm_objs): %.o : %.S
	$(CC) $(CFLAGS) -c $^ -o $@
crt0_objs += $(crt0_asm_objs)

crt0_c_srcs = $(filter %.c, $(crt0_srcs))
crt0_c_objs = $(patsubst %.c, %.o, $(notdir $(crt0_c_srcs)))
$(crt0_c_objs): %.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@
crt0_objs += $(crt0_c_objs)

crt0_lib = libcrt0.a
$(crt0_lib): $(crt0_objs)
	$(AR) rcv $@ $^
	$(RANLIB) $@

all: $(gloss_lib) $(crt0_lib)

clean: 
	rm -rf *.a *.o
