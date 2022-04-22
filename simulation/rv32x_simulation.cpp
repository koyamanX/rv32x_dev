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
#include "elfloader/elfloader.h"
#include <signal.h>
#include <string.h>
#include <byteswap.h>

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
#define START_ADDR 0x80000000
#define KERNEL_START_ADDR 0x80400000 // opensbi/plarform/rv32xsoc/config.mk: FW_JUMP_ADDR = 0x80400000
#define PAGE_OFFSET 0xc0000000		 //.config CONFIG_PAGE_OFFSET
#define VA_PA_OFFSET (PAGE_OFFSET - KERNEL_START_ADDR)

#define SATP 0x180
#define SATP_MODE 0x80000000

#define OPCODE 0x0000007F
#define JAL 111
#define JALR 103
#define AUIPC 23
#define MRET 0x30200073
#define SRET 0x10200073
#define RET 0x00008067
#define DEST 0x00000F80
#define RA 0x00000080

using namespace std;

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
		d[i + 0] = s[i + 2];
		d[i + 1] = s[i + 3];
		d[i + 2] = s[i + 0];
		d[i + 3] = s[i + 1];
	}
}

class processor_t
{
private:
	char *vcdfilename;
	const char *archname = "elf32-littleriscv";
	const char *exefilename = "a.out";
	const char *procname = NULL;
	char *logfilename = NULL;
	FILE *logfile;
	unsigned int imem_wait = 0;
	unsigned int dmem_wait = 0;
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
	int inst_counter = 1; //最初0x00000000なので、条件の都合上1
	int dump_vcd_flag = 0;
	long start_dump_inst = 0;
	long end_dump_inst = 0;
	int print_entry_flag = 0;
	struct symbol
	{
		unsigned int addr = 0;
		char *name = NULL;
		int dumpflag = 0;
	} * procedure, *globalObject;
	struct symbolCache
	{
		unsigned int addr = 0;
		int index = 0;
	} pCache[256 + 1], gOcache[256 + 1];
	char *target_symbol = NULL;
	int import_linux_symbol_flag = 0;
	const char *vmlinux_location = "/root/linux/vmlinux";
	int skip_procedure_search = 0;
	int skip_object_search = 0;
	unsigned char pcacheptr = 0;
	unsigned char gOcacheptr = 0;
	int callDepth = 0;
	int sv32_enabled = 0;

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
			block_device = fopen(BLOCK_DEVICE_FILENAME, "rb");
		}
#ifndef FAST_SIM
#endif
		regs = (uint32_t *)calloc(sizeof(uint32_t), 32);
	};
	~processor_t(void)
	{
#ifndef FAST_SIM
		if (dump_vcd_flag)
		{
			tfp->close();
		}
#endif
		core->final();
		fclose(logfile);
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
	void load(char *exefilename)
	{
		exefilename = exefilename;
		abfd = open_exe(exefilename, archname);
		load_elf(memory, abfd);
		print_mem_list(memory);
	};
	void resetCore(void)
	{
		p_reset = 0;
		reset = 0;

		while (tick() <= 10)
		{
			eval();
			dump();
		}
		/* assert reset signal for one clock cycle */
		{
			p_reset = 1;
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
#ifndef FAST_SIM
		if (dump_vcd_flag && ((start_dump_inst == 0 && end_dump_inst == 0) || (start_dump_inst <= inst_counter && end_dump_inst >= inst_counter)))
		{
			tfp->dump(m_clock_count);
		}
#endif
	};
	unsigned long long getSimTime(void)
	{
		return m_clock_count;
	};
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
				// static int temp = 0;
				/*
				if(core->uart_done) {
					fprintf(stdout, "%c", core->uart_data);
					fflush(stdout);
				}
				*/
				core->uart_wdata = 0;
				ch = getchar();
				if (ch != EOF)
				{
					/*
					 if (temp)
					 {
						 dump_vcd_flag = 1; // uart debug
					 }
					if (ch == '\n')
					{
						temp++;
					}
					 */
					core->uart_wdata = ch;
					// printf("ch:%x,wdata:%x\n", ch, core->uart_wdata);
					core->uart_write = 1;
				}
				else
				{
					core->uart_write = 0;
				}
			}
			if (rising_edge)
			{
				if (core->sim_done && !no_sim_exit)
				{
					ret = core->tohost;
				}
				if (core->debug_raise_exception)
				{
					epc = core->debug_epc;
					cause = core->debug_cause;
					mtval = core->debug_mtval;
					einst = core->debug_inst;

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
				if (core->debug_retire)
				{
					break;
				}
			}
		}

		if (retire_pc != core->debug_retire_pc)
		{
			inst_counter++;
		}
		if ((retire_inst & OPCODE) == JAL || (retire_inst & OPCODE) == JALR ||
			retire_inst == MRET || retire_inst == SRET || retire_inst == 0)
		{
			skip_procedure_search = 0;
		}
		else
		{
			skip_procedure_search = 1;
		}
		if ((retire_inst & OPCODE) == AUIPC)
		{
			skip_object_search = 0;
		}
		else
		{
			skip_object_search = 1;
		}
		retire_pc = core->debug_retire_pc;
		retire_inst = core->debug_retire_inst;
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
			retire_inst = einst;
		}
		if (core->debug_csr_write && core->debug_csr_write_num == SATP)
		{
			if (!sv32_enabled && (core->debug_csr_write_data & SATP_MODE))
			{
				for (int i = 0; procedure[i].addr != 0; i++)
				{
					procedure[i].addr += VA_PA_OFFSET;
				}
				for (int i = 0; globalObject[i].addr != 0; i++)
				{
					globalObject[i].addr += VA_PA_OFFSET;
				}
				sv32_enabled = 1;
			}
			else if (sv32_enabled && !(core->debug_csr_write_data & SATP_MODE))
			{
				for (int i = 0; procedure[i].addr != 0; i++)
				{
					procedure[i].addr -= VA_PA_OFFSET;
				}
				for (int i = 0; globalObject[i].addr != 0; i++)
				{
					globalObject[i].addr -= VA_PA_OFFSET;
				}
				sv32_enabled = 0;
			}
		}

		memset(core->funcname, 0, sizeof(__uint128_t));

		if (import_linux_symbol_flag && (retire_pc != KERNEL_START_ADDR))
		{
			skip_procedure_search = 1;
			skip_object_search = 1;
		}
		else
		{
			import_linux_symbol_flag = 0; // enable symbol search after payload transition to linux kernel
		}
		if (!import_linux_symbol_flag && retire_inst == RET)
		{
			// printf("ret:%x\n", retire_inst);
			callDepth--;
		}
		if (!skip_procedure_search)
		{
			int idx = -1;
			if ((idx = searchCache(retire_pc, pCache)) != -1 || (idx = searchSymbol(retire_pc, procedure, pCache, &pcacheptr)) != -1)
			{
				char tempstr[16] = {0};
				uint64_t funcName7to0 = 0;
				uint64_t funcName15to8 = 0;
				strncpy(tempstr, procedure[idx].name, 15);
				funcName7to0 = bswap_64(*(uint64_t *)(tempstr));
				funcName15to8 = bswap_64(*(uint64_t *)(tempstr + 8));
				*(uint64_t *)(core->funcname) = funcName15to8;
				*((uint64_t *)((core->funcname) + 2)) = funcName7to0;
				memset(tempstr, 0, 16);
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
		}
		if (!import_linux_symbol_flag &&
			((retire_inst != RET) && ((retire_inst & DEST) == RA) &&
			 (((retire_inst & OPCODE) == JAL) ||
			  ((retire_inst & OPCODE) == JALR))))
		{
			callDepth++;
		}
		if (trace_output_flag)
		{
			fprintf(logfile, "%s: 0x%016x (0x%08x) ", procname, retire_pc, retire_inst);
		}
		if ((disasm != NULL) && disasm_output_flag)
		{
			printDisasm(retire_pc, retire_inst);
			fprintf(logfile, "\n");
		}
		if (core->debug_mem_write && memory_output_flag)
		{
			int mask;
			mask = 0xffffffff >> (32 - (core->debug_mem_byteen + 1) * 8);
			fprintf(logfile, "\t");
			printMemWrite(core->debug_mem_adrs, core->debug_mem_data & mask);
		}
		if (core->debug_wb && writeback_output_flag)
		{
			fprintf(logfile, "\t");
			printRegInfo(core->debug_wb_rd, core->debug_wb_data);
		}
		if ((core->debug_mem_write && memory_output_flag) || (core->debug_wb && writeback_output_flag))
		{
			fprintf(logfile, "\n");
		}
		dumpRegs();

		return ret;
	};
	int searchCache(uint32_t data, struct symbolCache *cache)
	{
		for (int i = 0; cache[i].addr != 0; i++)
		{
			if (data == cache[i].addr)
			{
				return cache[i].index;
			}
		}
		return -1;
	};
	void setCache(struct symbolCache *cache, struct symbol *target, unsigned char ptr, unsigned char idx)
	{
		cache[ptr].addr = target[idx].addr;
		cache[ptr].index = idx;
	};
	int searchSymbol(uint32_t data, struct symbol *target, struct symbolCache *cache, unsigned char *cacheptr)
	{
		for (int i = 0; target[i].addr != 0; i++)
		{
			if (data == target[i].addr)
			{
				setCache(cache, target, *cacheptr++, i);
				return i;
			}
		}
		return -1;
	};
	void openLogFile(const char *s)
	{
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
		logfilename = (char *)calloc(sizeof(char), strlen(s) + strlen(".log"));
		strcat(logfilename, basename(s));
		strcat(logfilename, ".log");
		logfile = fopen(logfilename, "w");
	};
	void parseLogOpts(int argc, char **argv)
	{
		int opt, longindex;
		char *Darg = NULL, *larg = NULL, *symfile = NULL;
		struct option longopts[] = {
			{"print-exception", no_argument, NULL, 'e'},
			{"print-writeback", no_argument, NULL, 'w'},
			{"print-disasm", no_argument, NULL, 'd'},
			{"print-memory-write", no_argument, NULL, 'm'},
			{"print-inst-trace", no_argument, NULL, 'i'},
			{"print-all", no_argument, NULL, 'a'},
			{"no-log", no_argument, NULL, 'x'},
			{"no-sim-exit", no_argument, NULL, 'n'},
			{"dump-vcd", optional_argument, NULL, 'D'},
			{"print-entry", no_argument, NULL, 'E'},
			{"debug-linux", no_argument, NULL, 'l'},
			{0, 0, 0, 0},
		};
		while ((opt = getopt_long(argc, argv, "ewdmianxD::El", longopts, &longindex)) != -1)
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
				break;
			case 'n':
				no_sim_exit = 1;
				break;
			case 'x':
				no_log = 1;
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
		}

		if (print_all_flag)
		{
			exception_output_flag = 1;
			writeback_output_flag = 1;
			disasm_output_flag = 1;
			memory_output_flag = 1;
			trace_output_flag = 1;
			print_entry_flag = 1;
		}
		free(Darg);
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
				printf("illegal dump-vcd arg\nexample:\n --dump-vcd=s,<symbol name>\n --dump-vcd=i,<start(option)>-<end(option)>\n --dump-vcd\n");
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
					*bar = '-';
					filename = (char *)malloc(sizeof(char) * strlen(range) + strlen(fileExtension) + 1);
					strcpy(filename, range);
					vcdfilename = strcat(filename, fileExtension);
				}
			}
			else if (*Darg == 's') // start dumping vcd if pc equal to specified symbol address
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
					printf("--dump-vcd: target symbol %s does not exist\n", target_symbol);
					exit(1);
				}
				filename = (char *)malloc(sizeof(char) * strlen(target_symbol) + strlen(fileExtension) + 1);
				strcpy(filename, target_symbol);
				vcdfilename = strcat(filename, ".vcd");
				dump_vcd_flag = 0;
			}
			else
			{
				printf("illegal dump-vcd arg\nexample:\n --dump-vcd=s,<symbol name>\n --dump-vcd=i,<start(option)>-<end(option)>\n --dump-vcd\n");
				exit(1);
			}
		}
		else
		{
			char name[10] = "all";
			vcdfilename = strcat(name, ".vcd");
		}
		tfp = new VerilatedVcdC;
		core->trace(tfp, 99);
		tfp->open(vcdfilename);
		// dump_vcd_flag = 0; // uart debug
	}
	void fetchSymboltable(void)
	{
		long storage_needed;
		asymbol **symbol_table;
		long number_of_symbols;
		int pCounter = 0;
		int gOCounter = 0;
		int start_addr;
		if (import_linux_symbol_flag)
		{
			start_addr = KERNEL_START_ADDR;
		}
		else
		{
			start_addr = 0;
		}

		storage_needed = bfd_get_symtab_upper_bound(abfd);

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
			if ((symbol_table[i]->flags & BSF_OBJECT) && (symbol_table[i]->flags & BSF_GLOBAL))
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
				(!(symbol_table[i]->section->flags & SEC_DATA)))
			{
				procedure[pCounter].name = (char *)malloc(sizeof(char) * strlen(symbol_table[i]->name) + 1);
				procedure[pCounter].addr = start_addr + symbol_table[i]->section->lma + symbol_table[i]->value; // vma = start+lma+value
				strcpy(procedure[pCounter].name, symbol_table[i]->name);
				// printf("%d %x %s 0x%lx, flags=%x lma=%lx\n", pCounter, procedure[pCounter].addr, procedure[pCounter].name, symbol_table[i]->value, symbol_table[i]->flags, symbol_table[i]->section->lma);
				pCounter++;
			}
			if ((symbol_table[i]->flags & BSF_OBJECT) && (symbol_table[i]->flags & BSF_GLOBAL))
			{
				globalObject[gOCounter].name = (char *)malloc(sizeof(char) * strlen(symbol_table[i]->name) + 1);
				globalObject[gOCounter].addr = start_addr + symbol_table[i]->section->lma + symbol_table[i]->value;
				strcpy(globalObject[gOCounter].name, symbol_table[i]->name);
				// printf("%d %x %s 0x%lx, flags=%x\n", gOCounter, globalObject[gOCounter].addr, globalObject[gOCounter].name, symbol_table[i]->value, symbol_table[i]->flags);
				gOCounter++;
			}
			// printf("%d %s value=0x%lx flags=%x vma=%lx lma=%lx section_flags=%x\n", i, symbol_table[i]->name, symbol_table[i]->value, symbol_table[i]->flags, symbol_table[i]->section->vma, symbol_table[i]->section->lma, symbol_table[i]->section->flags);
		}
		// exit(1);
	};
	void openDisasm(void)
	{
		init_disassemble_info(&disasm_info, logfile, (fprintf_ftype)fprintf);
		disasm_info.arch = bfd_arch_riscv;
		disasm_info.mach = bfd_mach_riscv32;
		disasm_info.buffer_length = 0x4;
		disasm_info.buffer = instbuf;
		disasm_info.read_memory_func = buffer_read_memory;
		disassemble_init_for_target(&disasm_info);
		disasm = disassembler(bfd_arch_riscv, false, bfd_mach_riscv32, NULL);
	};
	void printDisasm(uint32_t pc, uint32_t inst)
	{
		memcpy(instbuf, &inst, sizeof(uint32_t));
		disasm_info.buffer_vma = pc;
		disasm(pc, &disasm_info);
	};
	void dumpRegs(void)
	{
		regs[0] = core->debug_pc;
		regs[1] = core->debug_x1;
		regs[2] = core->debug_x2;
		regs[3] = core->debug_x3;
		regs[4] = core->debug_x4;
		regs[5] = core->debug_x5;
		regs[6] = core->debug_x6;
		regs[7] = core->debug_x7;
		regs[8] = core->debug_x8;
		regs[9] = core->debug_x9;
		regs[10] = core->debug_x10;
		regs[11] = core->debug_x11;
		regs[12] = core->debug_x12;
		regs[13] = core->debug_x13;
		regs[14] = core->debug_x14;
		regs[15] = core->debug_x15;
		regs[16] = core->debug_x16;
		regs[17] = core->debug_x17;
		regs[18] = core->debug_x18;
		regs[19] = core->debug_x19;
		regs[20] = core->debug_x20;
		regs[21] = core->debug_x21;
		regs[22] = core->debug_x22;
		regs[23] = core->debug_x23;
		regs[24] = core->debug_x24;
		regs[25] = core->debug_x25;
		regs[26] = core->debug_x26;
		regs[27] = core->debug_x27;
		regs[28] = core->debug_x28;
		regs[29] = core->debug_x29;
		regs[30] = core->debug_x30;
		regs[31] = core->debug_x31;
	};
	void printRegInfo(uint8_t rd, uint32_t data)
	{
		fprintf(logfile, "%4s <- %08x", reg_strs[rd], data);
		if (!skip_object_search)
		{
			int idx = -1;
			if ((idx = searchCache(core->debug_wb_data, gOcache)) != -1 || (idx = searchSymbol(core->debug_wb_data, globalObject, gOcache, &gOcacheptr)) != -1)
			{
				fprintf(logfile, "(%s)", globalObject[idx].name);
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
	void flushPipeline(void){

	};
};

typedef struct
{
	struct termios *tmio;
	processor_t *procs;
} env_t;

void gotSigInt(int i)
{
	exit(0);
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

	env.procs = proc0;
	env.tmio = &stmio;
	on_exit(sim_exit, &env);

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	/*
	tcgetattr(STDIN_FILENO, &tmio);
	stmio = tmio;
	tmio.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSANOW, &tmio);
	*/

	Verilated::commandArgs(argc, argv);
	Verilated::traceEverOn(true);

	proc0 = new processor_t("core\t0");
	proc0->load(argv[1]); // elf
	proc0->parseLogOpts(argc - 1, argv + 1);
	proc0->openLogFile(argv[1]);
	proc0->openDisasm();

	proc0->resetCore();
	while (1)
	{
		ret = proc0->step();
		if (ret != -1)
		{
			break;
		}
	}
	proc0->step(); /* flush store instruction */

	return (ret == 0x00000001) ? 0 : ret;
}

void sim_exit(int status, void *p)
{
	env_t *env;
	env = (env_t *)p;
	// tcsetattr(STDIN_FILENO, TCSANOW, env->tmio);
	delete env->procs;
}
