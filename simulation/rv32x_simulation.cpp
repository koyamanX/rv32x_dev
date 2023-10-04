#define PACKAGE "bfd"
#include <bfd.h>
#include <iostream>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <dis-asm.h>
#include <verilated.h>
#include "verilated_vcd_c.h"
#include "Vrv32x_simulation.h"
#include "Vrv32x_simulation___024root.h"
#include "elfloader/elfloader.h"
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <byteswap.h>

#ifdef __cplusplus
extern "C"
{
#endif
	extern int print_insn_riscv(bfd_vma, disassemble_info *);
	extern disassembler_ftype riscv_get_disassembler(bfd *);
#ifdef __cplusplus
}
#endif

#define IMEM_WAIT 1
#define DMEM_WAIT 1

#define INSTRUCTION_ADDRESS_MISALIGNED 0
#define INSTRUCTION_ACCESS_FAULT 1
#define ILLEGAL_INSTRUCTION 2
#define BREAKPOINT 3
#define LOAD_ADDRESS_MISALIGNED 4
#define LOAD_ACCESS_FAULT 5
#define STORE_AMO_ADDRESS_MISALIGNED 6
#define STORE_AMO_ACCESS_FAULT 7
#define ENVIRONMENT_CALL_FROM_U_MODE 8
#define ENVIRONMENT_CALL_FROM_S_MODE 9
#define ENVIRONMENT_CALL_FROM_M_MODE 11
#define INSTRUCTION_PAGE_FAULT 12
#define LOAD_PAGE_FAULT 13
#define STORE_AMO_PAGE_FAULT 15
#define MACHINE_TIMER_INTERRUPT 0x80000007
#define MACHINE_SOFTWARE_INTERRUPT 0x80000003
#define MACHINE_EXTERNAL_INTERRUPT 0x8000000b
#define SUPERVISOR_TIMER_INTERRUPT 0x80000005
#define SUPERVISOR_SOFTWARE_INTERRUPT 0x80000001
#define SUPERVISOR_EXTERNAL_INTERRUPT 0x80000009

#define BLOCK_DEVICE_FILENAME "block_device.img"
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
#define START_ADDR 0x80000000
#ifndef KERNEL_START_ADDR
#define KERNEL_START_ADDR 0x80000000 // opensbi/plarform/rv32xsoc/config.mk: FW_JUMP_ADDR = 0x80400000
#endif

#define SATP 0x180
#define SATP_MODE 0x80000000

#define SET_VA 1
#define SET_PA 0

#define OPCODE 0x0000007F
#define BRANCH 99
#define JAL 111
#define JALR 103
#define AUIPC 23
#define LOAD 3
#define STORE 35
#define MRET 0x30200073
#define SRET 0x10200073
#define RET 0x00008067

#define MXLEN 32
#define RS2_OFFSET 20
#define RS1_OFFSET 15
#define DEST_OFFSET 7
#define FUNCT3_OFFSET 12
#define IMM_11_0_OFFSET 20
#define IMM_11_5_OFFSET 25
#define IMM_4_0_OFFSET 7

#define IMM_11_0 (0xFFF << IMM_11_0_OFFSET)
#define IMM_11_5 (0x7F << IMM_11_5_OFFSET)
#define IMM_4_0 (0x1F << IMM_4_0_OFFSET)
#define RS2 (0x1F << RS2_OFFSET)
#define RS1 (0x1F << RS1_OFFSET)
#define DEST (0x1F << DEST_OFFSET)
#define FUNCT3 (0x7 << FUNCT3_OFFSET)

#define SIGN_BIT(x) (-((unsigned)x >> (MXLEN - 1)))
#define LOAD_OFFSET(inst) ((SIGN_BIT(inst) ^ inst) >> IMM_11_0_OFFSET ^ SIGN_BIT(inst)) // arithmetic shift
#define STORE_OFFSET(inst) ((((SIGN_BIT(inst) ^ (inst & IMM_11_5)) >> IMM_11_0_OFFSET) ^ SIGN_BIT(inst)) + (((inst & IMM_4_0) >> IMM_4_0_OFFSET) & (IMM_4_0 >> IMM_4_0_OFFSET)))

#define RA 0x00000080

using namespace std;
// extern disassembler_ftype riscv_get_disassembler(bfd *);

const char *skip_procs[] = {
	"clear_bss",
	"___might_sleep",
	"__might_sleep",
	"do_raw_spin_unlock",
	"do_raw_spin_lock",
	"_raw_spin_unlock",
	"__cpuidle_text_end",
	"__memset",
	"__memcpy",
	"should_failslab",
	"mutex_unlock",
	"mutex_lock",
	"format_decode",
	"kmem_cache_alloc",
	"__list_add_valid",
	"rb_insert_color",
	"_raw_spin_unlock_irqrestore",
	"__raw_spin_lock_init",
	"kstrdup_const",
	"kernfs_name_hash",
	"delete_node",
	"__list_del_entry_valid",
	"__radix_tree_replace",
	"node_tag_clear",
	"idr_get_free",
	"set_iter_tags",
	"radix_tree_iter_replace",
	"radix_tree_iter_tag_clear",
	"idr_alloc_u32",
	"idr_alloc_cyclic",
	"idr_preload",
	"__radix_tree_preload.constprop.0",
	"riscv_clocksource_rdtime",
	"_raw_spin_lock_irqsave",
	"kernfs_new_node",
	"do_swap",
	"__virt_to_phys",
	"simple_strtoul",
	"_parse_integer",
	"_parse_integer_fixup_radix",
	"sbi_console_putchar",
	"jump_label_cmp",
	"_raw_spin_lock_irq",
	"_raw_spin_unlock_irq",
	NULL};

/*xv6	 							&& !((!strcmp(procedure[idx].name, "push_off")) ||
									  (!strcmp(procedure[idx].name, "pop_off")) ||
									  (!strcmp(procedure[idx].name, "mycpu")) ||
									  (!strcmp(procedure[idx].name, "acquire")) ||
									  (!strcmp(procedure[idx].name, "holding")) ||
									  (!strcmp(procedure[idx].name, "release")))*/

const char *abi_reg_strs[] = {
	"zero", "ra", "sp", "gp",
	"tp", "t0", "t1", "t2",
	"s0", "s1", "a0", "a1",
	"a2", "a3", "a4", "a5",
	"a6", "a7", "s2", "s3",
	"s4", "s5", "s6", "s7",
	"s8", "s9", "s10", "s11",
	"t3", "t4", "t5", "t6"};

void sim_exit(int status, void *p);
static void cpy(void *dest, void *src, size_t n)
{
	uint8_t *d, *s;

	d = (uint8_t *)dest, s = (uint8_t *)src;
	for (size_t i = 0; i < n; i += 4)
	{
		d[i + 0] = s[i + 3];
		d[i + 1] = s[i + 2];
		d[i + 2] = s[i + 1];
		d[i + 3] = s[i + 0];
	}
}

static void reverse(void *dest, void *src, size_t n)
{
	uint8_t *d = (uint8_t *)dest, *s = (uint8_t *)src;
	uint8_t x;
	for (size_t i = 0; i < n; i++)
	{
		x = s[i];
		x = (x & 0xF0) >> 4 | (x & 0x0F) << 4;
		x = (x & 0xCC) >> 2 | (x & 0x33) << 2;
		x = (x & 0xAA) >> 1 | (x & 0x55) << 1;
		d[i] = x;
	}
}

static void erase(FILE *block_device, unsigned head, unsigned tail, unsigned *erase_exclusion, unsigned excnt, unsigned group_flag)
{
	unsigned ptr;
	char *zero;
	unsigned exclude_flag = 0;
	unsigned erase_size = 0x200;
	if (group_flag)
	{
		erase_size = 0x10000;
	}
	zero = (char *)calloc(erase_size, sizeof(char));
	for (ptr = head; ptr < tail; ptr += erase_size)
	{
		fseek(block_device, ptr, SEEK_SET);
		for (int i = 0; i < excnt; i++)
		{
			if (ptr == erase_exclusion[excnt])
			{
				exclude_flag = 1;
				break;
			}
		}
		if (!exclude_flag)
		{
			fwrite(zero, sizeof(uint8_t), erase_size, block_device);
		}
		exclude_flag = 0;
	}
	free(zero);
}

static int fprintf_styled_func(void *stream, enum disassembler_style style,
							   const char *format, ...)
{
	// styleは色分けとかに使う？(riscv-gnu-toolchain/binutils/disasm.c参照)
	va_list va;
	va_start(va, format);
	vfprintf((FILE *)stream, format, va);
	va_end(va);
	return 0;
}

class processor_t
{
private:
	char *vcdfilename;
	const char *archname = "elf32-littleriscv";
	const char *exefilename = "a.out";
	const char *procname = NULL;
	char *logfilename = NULL;
	FILE *logfile = NULL;
	unsigned int m_clock_count = 0;
	uint32_t retire_pc = 0;
	uint32_t retire_inst = 0;
	uint32_t *regs;
	int reset = 0;
	int p_reset = 0;
	int rising_edge = 0;
	const char **reg_strs = abi_reg_strs;
	bfd *abfd;
	Vrv32x_simulation *core;
	VerilatedVcdC *tfp;
	memlist_t *memory;
	unsigned char instbuf[4];
	size_t buflen;
	disassembler_ftype disasm = NULL;
	struct disassemble_info disasm_info = {};
	int exception_output_flag = 0;
	int no_sim_exit = 0;
	int no_log = 0;
	int writeback_output_flag = 0;
	int disasm_output_flag = 0;
	int memory_output_flag = 0;
	int trace_output_flag = 0;
	int print_all_flag = 0;
	int priv_mode = 3;
	uint8_t buf[512];
	FILE *block_device;
	int block_device_avail = 0;
	unsigned int inst_counter = 1; // 最初0x00000000なので、条件の都合上1
	int dump_vcd_flag = 0;
	long start_dump_inst = 0;
	long end_dump_inst = 0;
	int print_entry_flag = 0;
	asymbol **symbol_table;
	long number_of_symbols;
	struct symbol
	{
		unsigned int addr = 0;
		unsigned int vma = 0;
		unsigned int pma = 0;
		char *name = NULL;
		int dumpflag = 0;
	} * procedure, *globalObject;
	char *target_symbol = NULL;
	int import_linux_symbol_flag = 0;
#ifdef KERNEL_ELF_LOCATION
	const char *vmlinux_location = STRINGIZE_VALUE_OF(KERNEL_ELF_LOCATION); //"/root/software/xv6-riscv/kernel/kernel"; //	"/root/software/busybox/busybox_unstripped"; //
#else
	const char *vmlinux_location = "/root/software/linux/vmlinux";
#endif
	int skip_procedure_search = 0;
	int skip_object_search = 0;
	int callDepth = 0;
	int sv32_enabled = 0;
	unsigned int callTracker[100] = {0};
	int print_blkrw_flag = 0;
	unsigned int nProc = 0;
	unsigned int nObject = 0;
	int dump_memory_flag = 0;
	long start_print_inst = 0;
	long end_print_inst = 0;
	int donePrintCondCheck = 1;
	int lsu_wcnt = 0;
	unsigned int mmc_erase_exclusion[16];

public:
	processor_t(const char *name)
	{
		procname = name;
		memory = init_mem_list();												  // memlistの先頭
		insert_new_mem(memory, "RAM0", 0x80000000, (0x84008000 - 0x80000000), 0); // memory->nextの指す先に格納
		core = new Vrv32x_simulation;
		if (access(BLOCK_DEVICE_FILENAME, F_OK) == 0)
		{
			block_device_avail = 1;
			block_device = fopen(BLOCK_DEVICE_FILENAME, "rb+");
		}
		regs = (uint32_t *)calloc(sizeof(uint32_t), 32);
	};
	~processor_t(void)
	{
		if (dump_vcd_flag)
		{
			tfp->close();
		}
		core->final();

#ifndef FAST_SIM
		if (logfile != NULL)
		{
			fclose(logfile);
		}
#endif
		if (block_device_avail)
		{
			fclose(block_device);
		}
	};
	const char *setExeFileName(const char *s)
	{
		exefilename = s;
		return exefilename;
	};
	void load(char *s)
	{
		abfd = open_exe(s, archname);
		load_elf(memory, abfd);
		print_mem_list(memory);
	};
	void resetCore(void)
	{
		p_reset = 0;
		reset = 0;

		while (tick() <= 2)
		{
			p_reset = !p_reset; // negedge reset
			eval();
			dump();
		}
		/* assert reset signal for one clock cycle */
		{
			reset = 1;
			eval();
			dump();

			tick();
			reset = 1;
			eval();
			dump();
		}
		/* negate reset signal */
		{
			tick();
			reset = 0;
			eval();
			dump();
		}
	};
	unsigned long long tick(void)
	{
		static int m_clock_prev = 0;

		core->reset = reset;
		core->p_reset = p_reset;
		m_clock_prev = core->m_clock;
		core->m_clock = !core->m_clock;
		rising_edge = ((core->m_clock == 1) && (m_clock_prev == 0));

		return m_clock_count++;
	};
	void eval(void)
	{
		core->eval();
	};
	void dump(void)
	{
		if (dump_vcd_flag && ((start_dump_inst == 0 && end_dump_inst == 0) || (start_dump_inst <= inst_counter && end_dump_inst >= inst_counter)))
		{
			tfp->dump(m_clock_count);
		}
	};
	unsigned long long getSimTime(void)
	{
		return m_clock_count;
	};
#ifndef FAST_SIM
	uint64_t step(void)
	{
		int imem_stat = 0;
		int dmem_stat = 0;
		static uint32_t epc, cause, mtval, einst, got_exception = -1;
		uint64_t ret = -1;
		static unsigned imem_read = 0;
		static unsigned dmem_read = 0;
		static unsigned dmem_write = 0;
		static unsigned dbyteen = 0;
		static unsigned wdata = 0;
		static unsigned iaddr = 0;
		static unsigned daddr = 0;
		static unsigned mem_addr, debug_mem_addr;
		static unsigned mem_byteen, debug_mem_byteen;
		static unsigned mem_data, debug_mem_data;
		static unsigned mem_write, debug_mem_write;
		static unsigned csr_write, debug_csr_write;
		static unsigned cwaddr, debug_cwaddr;
		static unsigned cwdata, debug_cwdata;

		while (1)
		{
			tick();
			eval();
			if (rising_edge)
			{
				core->inst = 0;
				core->imem_ready = 0;
				core->rdata = 0;
				core->dmem_ready = 0;
				if (imem_read)
				{
					imem_stat = read_word(memory, iaddr, (uint32_t *)&core->inst);
					core->imem_ready = 1;
				}
				if (dmem_read || dmem_write)
				{
					if ((dbyteen & 0x3) == 0)
					{
						if (dmem_read)
						{
							dmem_stat = read_byte(memory, daddr, (uint8_t *)&core->rdata);
						}
						else if (dmem_write)
						{
							dmem_stat = write_byte(memory, daddr, wdata);
						}
					}
					else if ((dbyteen & 0x3) == 1)
					{
						if (dmem_read)
						{
							dmem_stat = read_halfword(memory, daddr, (uint16_t *)&core->rdata);
						}
						else if (dmem_write)
						{
							dmem_stat = write_halfword(memory, daddr, wdata);
						}
					}
					else if ((dbyteen & 0x3) == 2)
					{
						if (dmem_read)
						{
							dmem_stat = read_word(memory, daddr, (uint32_t *)&core->rdata);
						}
						else if (dmem_write)
						{
							dmem_stat = write_word(memory, daddr, wdata);
						}
					}
					core->dmem_ready = 1;
				}
				if (imem_stat == -1)
				{
					fprintf(stderr, "Instruction Memory violation occuries at address of %08x\n", iaddr);
					fflush(stderr);
					fflush(stdout);
					exit(-1);
				}
				if (dmem_stat == -1)
				{
					fprintf(stderr, "Data memory violation occuries at address of %08x, byteen %02x\n", daddr, dbyteen);
					fflush(stderr);
					fflush(stdout);
					exit(-1);
				}
				iaddr = core->iaddr;
				daddr = core->daddr;
				imem_read = core->imem_read;
				dmem_read = core->dmem_read;
				dmem_write = core->dmem_write;
				wdata = core->wdata;
				dbyteen = core->dbyteen;

				if (block_device_avail)
				{
					if (core->read_block)
					{
						size_t len;

						fseek(block_device, core->block_adrs, SEEK_SET);
						len = fread(buf, sizeof(uint8_t), 512, block_device);
						if (print_blkrw_flag)
						{
							fprintf(logfile, "\nread mmc address 0x%x(block:0x%x), inst=%d\n", core->block_adrs, core->block_adrs / 512, inst_counter);
							for (int i = 0; i < 512; i += 4)
							{
								if (i % 16 == 0)
								{
									fprintf(logfile, "\n");
								}
								fprintf(logfile, "%02x%02x%02x%02x ", *(buf + i), *(buf + i + 1), *(buf + i + 2), *(buf + i + 3));
							}
							fprintf(logfile, "\n");
						}
						reverse(core->block_data, buf, 512);
						core->block_data_valid = 1;
					}
					else if (core->write_block)
					{
						fseek(block_device, core->block_adrs, SEEK_SET);
						cpy(buf, core->write_block_data, 512);
						if (print_blkrw_flag)
						{
							fprintf(logfile, "\nwrite mmc address 0x%x(block:0x%x), inst=%d\n", core->block_adrs, core->block_adrs / 512, inst_counter);
							for (int i = 0; i < 512; i += 4)
							{
								if (i % 16 == 0)
								{
									fprintf(logfile, "\n");
								}
								fprintf(logfile, "%02x%02x%02x%02x ", *(buf + i), *(buf + i + 1), *(buf + i + 2), *(buf + i + 3));
							}
							fprintf(logfile, "\n");
						}
						fwrite(buf, sizeof(uint8_t), 512, block_device);
					}
					else
					{
						core->block_data_valid = 0;
					}

					if (core->erase)
					{
						for (int i = 0; i < core->excnt; i++)
						{
							mmc_erase_exclusion[i] = (uint32_t)(core->exclusion)[i];
						}
						erase(block_device, core->head, core->tail, mmc_erase_exclusion, core->excnt, core->erase_group_en);
					}
				}
			}
			eval();
			dump();
			if (rising_edge)
			{
				ssize_t nr;
				int ch = -1;
				core->uart_wdata = 0;
				ch = getchar();
				if (ch != EOF)
				{
					core->uart_wdata = ch;
					core->uart_write = 1;
				}
				else
				{
					core->uart_write = 0;
				}
			}
			if (rising_edge)
			{
#ifdef HTIF
				if (core->sim_done && !no_sim_exit)
				{
					ret = core->tohost;
				}
#endif
				debug_mem_addr = mem_addr;
				debug_mem_byteen = mem_byteen;
				debug_mem_data = mem_data;
				debug_mem_write = mem_write;
				debug_csr_write = csr_write;
				debug_cwaddr = cwaddr;
				debug_cwdata = cwdata;
				mem_addr = core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___EMREG_alu_q;
				mem_byteen = core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___EMREG_funct3;
				mem_data = core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__lsu__DOT__cacheable_wdata;
				mem_write = core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___EMREG_store;
				csr_write = core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT___rv32x_csr_write;
				cwaddr = core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___EMREG_funct12;
				cwdata = core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT___rv32x_cwdata;

				if (core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__trap)
				{
					epc = core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__trap_pc;
					cause = core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__trap_cause;
					mtval = core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__trap_val;

					switch (cause)
					{
					case INSTRUCTION_ACCESS_FAULT:
					case INSTRUCTION_PAGE_FAULT:
					case ILLEGAL_INSTRUCTION:
						got_exception = 0;
						break;
					case INSTRUCTION_ADDRESS_MISALIGNED:
					case BREAKPOINT:
					case ENVIRONMENT_CALL_FROM_U_MODE:
					case ENVIRONMENT_CALL_FROM_S_MODE:
					case ENVIRONMENT_CALL_FROM_M_MODE:
					case LOAD_ADDRESS_MISALIGNED:
					case STORE_AMO_ADDRESS_MISALIGNED:
						got_exception = 1;
						break;
					case LOAD_ACCESS_FAULT:
					case STORE_AMO_ACCESS_FAULT:
					case LOAD_PAGE_FAULT:
					case STORE_AMO_PAGE_FAULT:
						got_exception = 0;
						break;
					case MACHINE_SOFTWARE_INTERRUPT:
					case MACHINE_TIMER_INTERRUPT:
					case MACHINE_EXTERNAL_INTERRUPT:
					case SUPERVISOR_SOFTWARE_INTERRUPT:
					case SUPERVISOR_TIMER_INTERRUPT:
					case SUPERVISOR_EXTERNAL_INTERRUPT:
					default:
						got_exception = 0;
						break;
					}
				}
				if (core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT__writeback)
				{
					break;
				}
			}
		}

		if (retire_pc != core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___MWREG_pc)
		{
			inst_counter++;
		}
		if (!donePrintCondCheck)
		{
			printCondCheck();
		}
		retire_pc = core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___MWREG_pc;
		retire_inst = core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___MWREG_inst;
		if ((got_exception == 0) && exception_output_flag)
		{
			printException(epc, cause, mtval);
			got_exception = -1;
		}
		else if (got_exception == -1)
		{
			;
		}
		else
		{
			got_exception--;
		}
		if (got_exception == 0)
		{
			retire_pc = epc;
		}
		if (debug_csr_write && debug_cwaddr == SATP)
		{
			if (!sv32_enabled && (debug_cwdata & SATP_MODE))
			{
				setSymbolAddr(SET_VA);
				sv32_enabled = 1;
			}
			else if (sv32_enabled && !(debug_cwdata & SATP_MODE))
			{
				setSymbolAddr(SET_PA);
				sv32_enabled = 0;
			}
		}

		if (print_entry_flag && retire_inst == RET)
		{
			if (callDepth != 0)
			{
				callDepth--;
			}
			if ((callTracker[callDepth] != core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT__rv32x_reg32__DOT__x1) && (callTracker[callDepth] != 0))
			{
				fprintf(logfile, "\nINFO:Call depth has been reset.(previous \"Depth=%d\" function return address(0x%x) discarded )\n", callDepth, callTracker[callDepth]);
				callDepth = 0;
				callTracker[callDepth] = 0;
			}
		}
		int idx = -1;
		if ((idx = searchSymbol(retire_pc, procedure)) != -1)
		{
			if (procedure[idx].dumpflag)
			{
				printf("\n\n%s reaches \"%s(%x)\" entry:start dumping %s\n\n", procname, target_symbol, retire_pc, vcdfilename);
				dump_vcd_flag = 1;
				procedure[idx].dumpflag = 0;
			}
			if (print_entry_flag)
			{
				fprintf(logfile, "\nDepth=%d, entry: %s(0x%x)", callDepth, procedure[idx].name, retire_pc);
				fprintf(logfile, "\tinst_counter = %d\n", inst_counter);
			}
		}
		if (print_entry_flag && ((retire_inst != RET) && ((retire_inst & DEST) == RA) &&
								 (((retire_inst & OPCODE) == JAL) ||
								  ((retire_inst & OPCODE) == JALR))))
		{
			// callDepth++;
			callTracker[callDepth++] = retire_pc + 4;
		}
		if (trace_output_flag)
		{
			fprintf(logfile, "inst_counter=%d, %s: 0x%016x (0x%08x) ", inst_counter, procname, retire_pc, retire_inst);
		}
		if ((disasm != NULL) && disasm_output_flag)
		{
			printDisasm(retire_pc, retire_inst);
			fprintf(logfile, "\n");
		}
		if (debug_mem_write && memory_output_flag)
		{
			int mask;
			mask = 0xffffffff >> (32 - (int)pow(2, (debug_mem_byteen & 0x3) + 3));
			fprintf(logfile, "\t");
			printMemWrite(debug_mem_addr, debug_mem_data & mask);
		}
		if (core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___MWREG_wb)
		{
			regs[core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___MWREG_rd] =
				core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___MWREG_wb_value;
			if (writeback_output_flag)
			{
				fprintf(logfile, "\t");
				printRegInfo(core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___MWREG_rd,
							 core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___MWREG_wb_value);
			}
		}
		if ((debug_mem_write && memory_output_flag) ||
			(core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___MWREG_wb && writeback_output_flag))
		{
			fprintf(logfile, "\n");
		}
		if (retire_pc == KERNEL_START_ADDR)
		{
			printf("Done bootloading\n");
		}

		return ret;
	};
#else
	uint64_t step(void)
	{
		int imem_stat = 0;
		int dmem_stat = 0;
		static uint32_t epc, cause, mtval, einst, got_exception = -1;
		uint64_t ret = -1;

		while (1)
		{
			tick();
			eval();
			core->inst = 0;
			if (core->imem_read)
			{
				if (imem_wait == IMEM_WAIT)
				{
					imem_stat = read_word(memory, core->iaddr, (uint32_t *)&core->inst);
					core->imem_ready = 1;
					imem_wait = 0;
				}
				else
				{
					core->imem_ready = 0;
					imem_wait++;
				}
			}
			else
			{
				core->imem_ready = 0;
			}
			core->rdata = 0;
			if (core->dmem_read || core->dmem_write)
			{
				if (dmem_wait == DMEM_WAIT)
				{
					if ((core->dbyteen & 0x3) == 0)
					{
						if (core->dmem_read)
						{
							dmem_stat = read_byte(memory, core->daddr, (uint8_t *)&core->rdata);
						}
						else if (core->dmem_write)
						{
							dmem_stat = write_byte(memory, core->daddr, core->wdata);
						}
					}
					else if ((core->dbyteen & 0x3) == 1)
					{
						if (core->dmem_read)
						{
							dmem_stat = read_halfword(memory, core->daddr, (uint16_t *)&core->rdata);
						}
						else if (core->dmem_write)
						{
							dmem_stat = write_halfword(memory, core->daddr, core->wdata);
						}
					}
					else if ((core->dbyteen & 0x3) == 2)
					{
						if (core->dmem_read)
						{
							dmem_stat = read_word(memory, core->daddr, (uint32_t *)&core->rdata);
						}
						else if (core->dmem_write)
						{
							dmem_stat = write_word(memory, core->daddr, core->wdata);
						}
					}
					core->dmem_ready = 1;
					dmem_wait = 0;
				}
				else
				{
					core->dmem_ready = 0;
					dmem_wait++;
				}
			}
			else
			{
				core->dmem_ready = 0;
			}
			if (imem_stat == -1)
			{
				fprintf(stderr, "Instruction Memory violation occuries at address of %08x, byteen %02x\n", core->iaddr, core->ibyteen);
				fflush(stderr);
				fflush(stdout);
				exit(-1);
			}
			if (dmem_stat == -1)
			{
				fprintf(stderr, "Data memory violation occuries at address of %08x, byteen %02x\n", core->daddr, core->dbyteen);
				fflush(stderr);
				fflush(stdout);
				exit(-1);
			}
			if (rising_edge)
			{
				if (block_device_avail)
				{
					if (core->read_block)
					{
						size_t len;
						fseek(block_device, core->block_adrs * 512, SEEK_SET);
						len = fread(buf, sizeof(uint8_t), 512, block_device);
						cpy(core->block_data, buf, 512);
						core->block_data_valid = 1;
					}
					else if (core->write_block)
					{
						fseek(block_device, core->block_adrs * 512, SEEK_SET);
						cpy(buf, core->write_block_data, 512);
						fwrite(buf, sizeof(uint8_t), 512, block_device);
					}
					else
					{
						core->block_data_valid = 0;
					}
				}
			}
			eval();
			dump();
			if (rising_edge)
			{
				ssize_t nr;
				int ch = -1;
				core->uart_wdata = 0;
				ch = getchar();
				if (ch != EOF)
				{
					core->uart_wdata = ch;
					core->uart_write = 1;
				}
				else
				{
					core->uart_write = 0;
				}
			}
#ifdef HTIF
			if (rising_edge)
			{
				if (core->sim_done)
				{
					ret = core->tohost;
					break;
				}
			}
#endif
		}
		return ret;
	};
#endif

	static int compareAddr(const struct symbol *a, const struct symbol *b)
	{
		return a->addr - b->addr;
	};
	int searchSymbol(const uint32_t data, const struct symbol *target)
	{
		struct symbol key;
		struct symbol *res = NULL;

		key.addr = data;
		res = (struct symbol *)bsearch(&key, target, (target == procedure) ? nProc : nObject, sizeof(struct symbol), (comparison_fn_t)compareAddr);
		if (res != NULL)
		{
			return res - target;
		}
		return -1;
	};
#ifndef FAST_SIM
	void printRegInfo(uint8_t rd, uint32_t data)
	{
		fprintf(logfile, "%4s <- %08x", reg_strs[rd], data);
		int idx = -1;
		if ((idx = searchSymbol(core->rootp->rv32x_simulation__DOT__core__DOT__core__DOT__rv32x__DOT___MWREG_wb_value, globalObject)) != -1)
		{
			fprintf(logfile, "(%s)", globalObject[idx].name);
		}
	};
#endif
	void openLogFile(const char *s)
	{
		FILE *fp;
		if (no_log)
		{
			logfilename = (char *)"/dev/null";
			return;
		}
		if (logfilename != NULL)
		{
			free(logfilename);
			logfilename = NULL;
		}
#ifndef NO_TARGET
		logfilename = (char *)calloc(sizeof(char), strlen(basename(s)) + strlen(".log") + 1);
		strcpy(logfilename, basename(s));
		strcat(logfilename, ".log");
		fp = fopen(logfilename, "w");
		logfile = fp;
#else
		logfile = fopen("noTarget.log", "w");
#endif
	};
	void parseLogOpts(int argc, char **argv)
	{
		int opt, longindex;
		char *Darg = NULL, *aarg = NULL, *symfile = NULL;
		struct option longopts[] = {
			{"print-exception", no_argument, NULL, 'e'},
			{"print-writeback", no_argument, NULL, 'w'},
			{"print-disasm", no_argument, NULL, 'd'},
			{"print-memory-write", no_argument, NULL, 'm'},
			{"print-inst-trace", no_argument, NULL, 'i'},
			{"print-all", optional_argument, NULL, 'a'},
			{"no-sim-exit", no_argument, NULL, 'n'},
			{"dump-vcd", optional_argument, NULL, 'D'},
			{"print-entry", no_argument, NULL, 'E'},
			{"debug-linux", no_argument, NULL, 'l'},
			{"print-blkrw", no_argument, NULL, 'b'},
			{"dump-memory", no_argument, NULL, 'M'},
			{0, 0, 0, 0},
		};
		while ((opt = getopt_long(argc, argv, "ewdmianD::ElbM", longopts, &longindex)) != -1)
		{
			switch (opt)
			{
			case 'e':
				exception_output_flag = 1;
				break;
			case 'w':
				writeback_output_flag = 1;
				break;
			case 'd':
				disasm_output_flag = 1;
				break;
			case 'm':
				memory_output_flag = 1;
				break;
			case 'i':
				trace_output_flag = 1;
				break;
			case 'a':
				print_all_flag = 1;
				if (optarg != NULL)
				{
					aarg = (char *)malloc(sizeof(char) * strlen(optarg) + 1);
					strcpy(aarg, optarg);
				}
				break;
			case 'n':
				no_sim_exit = 1;
				break;
			case 'D':
				dump_vcd_flag = 1;
				if (optarg != NULL)
				{
					Darg = (char *)malloc(sizeof(char) * strlen(optarg) + 1);
					strcpy(Darg, optarg);
				}
				break;
			case 'E':
				print_entry_flag = 1;
				break;
			case 'l':
				import_linux_symbol_flag = 1;
				break;
			case 'b':
				print_blkrw_flag = 1;
				break;
			case 'M':
				dump_memory_flag = 1;
				break;
			default:
				exit(1);
				break;
			}
		}
		if (import_linux_symbol_flag)
		{
			abfd = open_exe(vmlinux_location, archname);
			skip_procedure_search = 1;
		}
		fetchSymboltable();
		if (dump_vcd_flag)
		{
			dumpSetting(Darg);
			free(Darg);
		}
		if (print_all_flag)
		{
			printSetting(aarg);
		}
	};
	void setSymbolAddr(int flag)
	{
		if (flag == SET_VA)
		{
			for (int i = 0; procedure[i].addr != 0; i++)
			{
				procedure[i].addr = procedure[i].vma;
			}
			for (int i = 0; globalObject[i].addr != 0; i++)
			{
				globalObject[i].addr = globalObject[i].vma;
			}
		}
		else if (flag == SET_PA)
		{
			for (int i = 0; procedure[i].addr != 0; i++)
			{
				procedure[i].addr = procedure[i].pma;
			}
			for (int i = 0; globalObject[i].addr != 0; i++)
			{
				globalObject[i].addr = globalObject[i].pma;
			}
		}
	};
	void printCondCheck()
	{
		if (start_print_inst == inst_counter)
		{
			exception_output_flag = 1;
			writeback_output_flag = 1;
			disasm_output_flag = 1;
			memory_output_flag = 1;
			trace_output_flag = 1;
			print_entry_flag = 1;
			print_blkrw_flag = 1;
			printf("startPrint\n");
		}
		else if (end_print_inst == inst_counter)
		{
			exception_output_flag = 0;
			writeback_output_flag = 0;
			disasm_output_flag = 0;
			memory_output_flag = 0;
			trace_output_flag = 0;
			print_entry_flag = 0;
			print_blkrw_flag = 0;

			donePrintCondCheck = 1;
			printf("endPrint\n");
		}
	};
	void printSetting(char *aarg)
	{
		if (aarg != NULL)
		{
			donePrintCondCheck = 0;
			char *comma;
			comma = strstr(aarg, ",");
			if (comma == NULL || comma != aarg + 1 || *(aarg + 2) == '\0')
			{
				printf("illegal print-all arg\nexample:\n --print-all=i,<start(option)>-<end(option)>\n --print-all\n");
				exit(1);
			}
			else if (*aarg == 'i')
			{
				char *range = NULL, *bar = NULL;
				range = aarg + 2;
				bar = strstr(range, "-");
				if (bar != NULL)
				{
					*bar = '\0';
					start_print_inst = strtol(range, NULL, 0);
					end_print_inst = strtol(bar + 1, NULL, 0);
					if (start_print_inst == 0 && end_print_inst != 0)
					{
						start_print_inst = 1;
					}
					else if (start_print_inst != 0 && end_print_inst == 0)
					{
						end_print_inst = start_print_inst + 10000;
					}
					*bar = '-';
				}
			}
			else
			{
				printf("illegal print-all arg\nexample:\n --print-all=i,<start(option)>-<end(option)>\n --print-all\n");
				exit(1);
			}
		}
		else
		{
			exception_output_flag = 1;
			writeback_output_flag = 1;
			disasm_output_flag = 1;
			memory_output_flag = 1;
			trace_output_flag = 1;
			print_entry_flag = 1;
			print_blkrw_flag = 1;
		}
	};
	void dumpSetting(char *Darg)
	{
		char *filename = NULL;
		const char *fileExtension = ".vcd";
		if (Darg != NULL)
		{
			char *comma;
			comma = strstr(Darg, ",");
			if (comma == NULL || comma != Darg + 1 || *(Darg + 2) == '\0')
			{
				printf("illegal dump-vcd arg\nexample:\n --dump-vcd=p,<procedure name(FUNC or NOTYPE symbol)>\n --dump-vcd=i,<start(option)>-<end(option)>\n --dump-vcd\n");
				exit(1);
			}
			else if (*Darg == 'i')
			{
				char *range = NULL, *bar = NULL;
				range = Darg + 2;
				bar = strstr(range, "-");
				if (bar != NULL)
				{
					*bar = '\0';
					start_dump_inst = strtol(range, NULL, 0);
					end_dump_inst = strtol(bar + 1, NULL, 0);
					if (start_dump_inst == 0 && end_dump_inst != 0)
					{
						start_dump_inst = end_dump_inst - 100;
					}
					else if (start_dump_inst != 0 && end_dump_inst == 0)
					{
						end_dump_inst = start_dump_inst + 100;
					}
					*bar = '-';
					filename = (char *)malloc(sizeof(char) * strlen(range) + strlen(fileExtension) + 1);
					strcpy(filename, range);
					vcdfilename = strcat(filename, fileExtension);
				}
			}
			else if (*Darg == 'p') // start dumping vcd if pc equal to specified symbol address
			{
				int i = 0;
				target_symbol = (char *)malloc(sizeof(char) * strlen(Darg + 2) + 1);
				strcpy(target_symbol, Darg + 2);
				for (i = 0; procedure[i].addr != 0; i++)
				{
					if (!strcmp(procedure[i].name, target_symbol))
					{
						procedure[i].dumpflag = 1;
						break;
					}
				}
				if (procedure[i].addr == 0)
				{
					printf("--dump-vcd: target procedure %s does not exist\n", target_symbol);
					exit(1);
				}
				filename = (char *)malloc(sizeof(char) * strlen(target_symbol) + strlen(fileExtension) + 1);
				strcpy(filename, target_symbol);
				vcdfilename = strcat(filename, fileExtension);
				dump_vcd_flag = 0;
			}
			else
			{
				printf("illegal dump-vcd arg\nexample:\n --dump-vcd=p,<procedure name(FUNC or NOTYPE symbol)>\n --dump-vcd=i,<start(option)>-<end(option)>\n --dump-vcd\n");
				exit(1);
			}
			printf("start=%ld,\nend=%ld\n", start_dump_inst, end_dump_inst);
		}
		else
		{
			vcdfilename = (char *)malloc(sizeof(char) * 12);
			strcpy(vcdfilename, "1-10000.vcd");
			start_dump_inst = 1;
			end_dump_inst = 10000;
		}
		tfp = new VerilatedVcdC;
		core->trace(tfp, 99);
		tfp->open(vcdfilename);
		// dump_vcd_flag = 0; // uart debug
	}

	int skipLogging(const char *proc)
	{
		const char **p = skip_procs;
		while (*p != NULL)
		{
			if (!strcmp(proc, *p))
			{
				return 1;
			}
			p++;
		}
		return 0;
	}

	void fetchSymboltable(void)
	{
		long storage_needed = 0;
		int pCounter = 0;
		int gOCounter = 0;
		unsigned int start_addr;
		if (import_linux_symbol_flag)
		{
			start_addr = KERNEL_START_ADDR; // 0; //
		}
		else
		{
			start_addr = 0;
		}
		if (abfd != NULL)
		{
			storage_needed = bfd_get_symtab_upper_bound(abfd);
		}

		if (storage_needed < 0) // failed reading symbol
		{
			return;
		}

		if (storage_needed == 0) // no symbol
		{
			return;
		}
		symbol_table = (asymbol **)malloc(storage_needed);
		number_of_symbols = bfd_canonicalize_symtab(abfd, symbol_table);

		if (number_of_symbols < 0) // failed reading symbol
		{
			return;
		}

		for (int i = 0; i < number_of_symbols; i++)
		{
			if (symbol_table[i]->value != 0 && !(symbol_table[i]->flags & (BSF_FILE | BSF_OBJECT)))
			{
				pCounter++;
			}
			if ((symbol_table[i]->flags & BSF_OBJECT) && ((symbol_table[i]->flags & BSF_GLOBAL) ||
														  ((symbol_table[i]->flags & BSF_LOCAL) &&
														   (strstr(symbol_table[i]->name, "__") == NULL) &&
														   (strstr(symbol_table[i]->name, ".") == NULL))))
			{
				gOCounter++;
			}
		}
		procedure = (struct symbol *)malloc(sizeof(symbol) * (pCounter + 1));
		globalObject = (struct symbol *)malloc(sizeof(symbol) * (gOCounter + 1));
		memset(procedure, 0, sizeof(symbol) * (pCounter + 1));
		memset(globalObject, 0, sizeof(symbol) * (gOCounter + 1));
		pCounter = 0;
		gOCounter = 0;
		for (int i = 0; i < number_of_symbols; i++)
		{
			if (symbol_table[i]->value != 0 && !(symbol_table[i]->flags & (BSF_FILE | BSF_OBJECT)) &&
				(!(symbol_table[i]->section->flags & SEC_DATA)) && !skipLogging(symbol_table[i]->name))
			{
				procedure[pCounter].name = (char *)malloc(sizeof(char) * strlen(symbol_table[i]->name) + 1);
#ifndef NO_TARGET
				procedure[pCounter].pma = start_addr + symbol_table[i]->section->lma + symbol_table[i]->value;
#else
				procedure[pCounter].pma = start_addr + symbol_table[i]->value;
#endif
				procedure[pCounter].vma = symbol_table[i]->section->vma + symbol_table[i]->value;
				procedure[pCounter].addr = procedure[pCounter].pma;
				strcpy(procedure[pCounter].name, symbol_table[i]->name);
				// printf("%d %08x name=%s value=0x%lx, flags=%x vma=%08lx lma=%08lx\n", pCounter, procedure[pCounter].addr, procedure[pCounter].name, symbol_table[i]->value, symbol_table[i]->flags, symbol_table[i]->section->lma, symbol_table[i]->section->lma);
				pCounter++;
			}
			if ((symbol_table[i]->flags & BSF_OBJECT) && ((symbol_table[i]->flags & BSF_GLOBAL) ||
														  ((symbol_table[i]->flags & BSF_LOCAL) &&
														   (strstr(symbol_table[i]->name, "__") == NULL) &&
														   (strstr(symbol_table[i]->name, ".") == NULL))))
			{
				globalObject[gOCounter].name = (char *)malloc(sizeof(char) * strlen(symbol_table[i]->name) + 1);
#ifndef NO_TARGET
				globalObject[gOCounter].pma = start_addr + symbol_table[i]->section->lma + symbol_table[i]->value;
#else
				globalObject[gOCounter].pma = start_addr + symbol_table[i]->value;
#endif
				globalObject[gOCounter].vma = symbol_table[i]->section->vma + symbol_table[i]->value;
				globalObject[gOCounter].addr = globalObject[gOCounter].pma;
				strcpy(globalObject[gOCounter].name, symbol_table[i]->name);
				// printf("%d %x %s 0x%lx, flags=%x\n", gOCounter, globalObject[gOCounter].addr, globalObject[gOCounter].name, symbol_table[i]->value, symbol_table[i]->flags);
				gOCounter++;
			}
			// printf("%d %s value=0x%lx flags=%x vma=%lx lma=%lx section_flags=%x\n", i, symbol_table[i]->name, symbol_table[i]->value, symbol_table[i]->flags, symbol_table[i]->section->vma, symbol_table[i]->section->lma, symbol_table[i]->section->flags);
		}
		nProc = pCounter;
		nObject = gOCounter;
		qsort(procedure, nProc, sizeof(struct symbol), (comparison_fn_t)compareAddr);
		qsort(globalObject, nObject, sizeof(struct symbol), (comparison_fn_t)compareAddr);
		for (int i = 0; i < nProc; i++)
		{
			// printf("%d pma:%x vma:%x %s\n", i, procedure[i].pma, procedure[i].vma, procedure[i].name);
		}
		// printf("n %d/n", nProc);
		// exit(1);
	};
	void openDisasm(void)
	{
		fprintf_styled_ftype printInsn = fprintf_styled_func;
		init_disassemble_info(&disasm_info, logfile, (fprintf_ftype)fprintf, printInsn);
		disasm_info.arch = bfd_arch_riscv;
		disasm_info.mach = bfd_mach_riscv32;
		disasm_info.buffer_length = 0x4;
		disasm_info.buffer = instbuf;
		disasm_info.read_memory_func = buffer_read_memory;
		disassemble_init_for_target(&disasm_info);
		disasm = disassembler(bfd_arch_riscv, false, bfd_mach_riscv32, NULL);
		// disasm = riscv_get_disassembler(abfd); // same above
	};
	void printDisasm(uint32_t pc, uint32_t inst)
	{
		memcpy(instbuf, &inst, sizeof(uint32_t));
		disasm_info.buffer_vma = pc;
		disasm(pc, &disasm_info);
		// print_insn_riscv(pc, &disasm_info);
		int idx = -1;
		int rs1 = (inst & RS1) >> RS1_OFFSET;
		int rs2 = (inst & RS2) >> RS2_OFFSET;
		int dest = (inst & DEST) >> DEST_OFFSET;
		int offset;
		if ((inst & OPCODE) == LOAD)
		{
			offset = LOAD_OFFSET(inst);
			if ((idx = searchSymbol(regs[rs1] + offset, globalObject)) != -1)
			{
				fprintf(logfile, "\t< %s=*(%s) >", reg_strs[dest], globalObject[idx].name);
			}
			else if ((idx = searchSymbol(regs[rs1], globalObject)) != -1)
			{
				fprintf(logfile, "\t< %s=*(%s %c %d) >", reg_strs[dest], globalObject[idx].name, (offset < 0) ? '-' : '+', (offset < 0) ? -offset : offset);
			}
		}
		else if ((inst & OPCODE) == STORE)
		{
			offset = STORE_OFFSET(inst);
			if ((idx = searchSymbol(regs[rs1] + offset, globalObject)) != -1)
			{
				fprintf(logfile, "\t< *(%s)=%s >", globalObject[idx].name, reg_strs[rs2]);
			}
			else if ((idx = searchSymbol(regs[rs1], globalObject)) != -1)
			{
				fprintf(logfile, "\t< *(%s %c %d)=%s >", globalObject[idx].name, (offset < 0) ? '-' : '+', (offset < 0) ? -offset : offset, reg_strs[rs2]);
			}
		}
	};
	void printException(uint32_t epc, uint32_t cause, uint32_t mtval)
	{
		const char *str;

#define STR(e) (#e)
		switch (cause)
		{
		case INSTRUCTION_ADDRESS_MISALIGNED:
			str = STR(INSTRUCTION_ADDRESS_MISALIGNED);
			break;
		case INSTRUCTION_ACCESS_FAULT:
			str = STR(INSTRUCTION_ACCESS_FAULT);
			break;
		case ILLEGAL_INSTRUCTION:
			str = STR(ILLEGAL_INSTRUCTION);
			break;
		case BREAKPOINT:
			str = STR(BREAKPOINT);
			break;
		case LOAD_ADDRESS_MISALIGNED:
			str = STR(LOAD_ADDRESS_MISALIGNED);
			break;
		case LOAD_ACCESS_FAULT:
			str = STR(LOAD_ACCESS_FAULT);
			break;
		case STORE_AMO_ADDRESS_MISALIGNED:
			str = STR(STORE_AMO_ADDRESS_MISALIGNED);
			break;
		case STORE_AMO_ACCESS_FAULT:
			str = STR(STORE_AMO_ACCESS_FAULT);
			break;
		case ENVIRONMENT_CALL_FROM_U_MODE:
			str = STR(ENVIRONMENT_CALL_FROM_U_MODE);
			break;
		case ENVIRONMENT_CALL_FROM_S_MODE:
			str = STR(ENVIRONMENT_CALL_FROM_S_MODE);
			break;
		case ENVIRONMENT_CALL_FROM_M_MODE:
			str = STR(ENVIRONMENT_CALL_FROM_M_MODE);
			break;
		case INSTRUCTION_PAGE_FAULT:
			str = STR(INSTRUCTION_PAGE_FAULT);
			break;
		case LOAD_PAGE_FAULT:
			str = STR(LOAD_PAGE_FAULT);
			break;
		case STORE_AMO_PAGE_FAULT:
			str = STR(STORE_AMO_PAGE_FAULT);
			break;
		case MACHINE_SOFTWARE_INTERRUPT:
			str = STR(MACHINE_SOFTWARE_INTERRUPT);
			break;
		case MACHINE_TIMER_INTERRUPT:
			str = STR(MACHINE_TIMER_INTERRUPT);
			break;
		case MACHINE_EXTERNAL_INTERRUPT:
			str = STR(MACHINE_EXTERNAL_INTERRUPT);
			break;
		case SUPERVISOR_SOFTWARE_INTERRUPT:
			str = STR(SUPERVISOR_SOFTWARE_INTERRUPT);
			break;
		case SUPERVISOR_TIMER_INTERRUPT:
			str = STR(SUPERVISOR_TIMER_INTERRUPT);
			break;
		case SUPERVISOR_EXTERNAL_INTERRUPT:
			str = STR(SUPERVISOR_EXTERNAL_INTERRUPT);
			break;
		default:
			str = "ILLEGAL_EXCEPTION";
			break;
		}
#undef STR

		fprintf(logfile, "%s\tepc <- %08x, mtval <- %08x\n", str, epc, mtval);
		// printDisasm(epc, mtval);
		// fprintf(logfile, ")\n");
	};
	void printMemWrite(uint32_t adrs, uint32_t data)
	{
		fprintf(logfile, "Memory[%08x] <- %08x", adrs, data);
	};
	void dumpMemory(void)
	{
		if (dump_memory_flag)
		{
			FILE *fp = fopen("RAM0_80000000-84008000.dat", "wb");
			fwrite(memory->next->mem, sizeof(uint8_t), memory->next->length, fp);
		}
	};
	void flushPipeline(void){

	};
};

typedef struct
{
	struct termios *tmio;
	processor_t **procs;
} env_t;

void gotSigInt(int i)
{
	exit(0);
}

void test()
{
	printf("\nhi\n");
}

int main(int argc, char **argv)
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = gotSigInt;
	sigaction(SIGINT, &act, NULL);

	int ret = 0;
	processor_t *proc0;
	struct termios tmio;
	struct termios stmio;
	env_t env;
	proc0 = NULL;
	env.tmio = &stmio;
	env.procs = &proc0;

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	tcgetattr(STDIN_FILENO, &tmio);
	stmio = tmio;
	tmio.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSANOW, &tmio);
	on_exit(sim_exit, &env);
	// Verilated::commandArgs(argc, argv);
	Verilated::traceEverOn(true);
	proc0 = new processor_t("core\t0");
#ifndef NO_TARGET
	proc0->load(argv[1]); // elf
#endif
#ifndef NO_TARGET
	proc0->parseLogOpts(argc - 1, argv + 1);
#else
	proc0->parseLogOpts(argc, argv);
#endif
#ifndef FAST_SIM
	proc0->openLogFile(argv[1]);
#endif
	proc0->openDisasm();
	proc0->resetCore();
	// printf("%p %p\n", *(env.procs), proc0);
	int cnt = 0;
	while (cnt < 1)
	{
		ret = proc0->step();
		if (ret != -1)
		{
			break;
		}
	}
#ifndef FAST_SIM
	proc0->step(); /* flush store instruction */
#endif

	return (ret == 0x00000001) ? 0 : ret;
}

void sim_exit(int status, void *p)
{
	env_t *env;
	env = (env_t *)p;
	tcsetattr(STDIN_FILENO, TCSANOW, env->tmio);
	if ((*(env->procs)) != NULL)
	{
		(*(env->procs))->dumpMemory();
		delete *(env->procs);
	}
}
