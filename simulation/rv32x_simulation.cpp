#define PACKAGE "bfd"
#include <bfd.h>
#include <iostream>
#include <dis-asm.h>
#include <verilated.h>
#include "verilated_vcd_c.h"
#include "Vrv32x_simulation.h"
#include "elfloader/elfloader.h"

#define IMEM_WAIT 5
#define DMEM_WAIT 5

#define INSTRUCTION_ADDRESS_MISALIGNED 0
#define INSTRUCTION_ACCESS_FAULT        1
#define ILLEGAL_INSTRUCTION             2
#define BREAKPOINT                      3
#define LOAD_ADDRESS_MISALIGNED         4
#define LOAD_ACCESS_FAULT               5
#define STORE_AMO_ADDRESS_MISALIGNED    6
#define STORE_AMO_ACCESS_FAULT          7
#define ENVIRONMENT_CALL_FROM_U_MODE    8
#define ENVIRONMENT_CALL_FROM_S_MODE    9
#define ENVIRONMENT_CALL_FROM_M_MODE    11
#define INSTRUCTION_PAGE_FAULT          12
#define LOAD_PAGE_FAULT                 13
#define STORE_AMO_PAGE_FAULT            15



using namespace std;

const char *abi_reg_strs[] = {
	"zero", "ra", "sp", "gp",
	"tp", "t0", "t1", "t2", 
	"s0", "s1", "a0", "a1", 
	"a2", "a3", "a4", "a5",
	"a6", "a7", "s2", "s3",
	"s4", "s5", "s6", "s7",
	"s8", "s9", "s10", "s11",
	"t3", "t4", "t5", "t6"
};


class processor_t {
private:
	const char *vcdfilename = "rv32x_simulation.vcd";
	const char *archname = "elf32-littleriscv";
	const char *exefilename = "a.out";
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
public:
	processor_t() {
		memory = init_mem_list();
		insert_new_mem(memory, "RAM0", 0x80000000, (0x84008000-0x80000000), 0);
		print_mem_list(memory);
		core = new Vrv32x_simulation;	
		tfp = new VerilatedVcdC;
		core->trace(tfp, 99);
		tfp->open(vcdfilename);
		regs = (uint32_t *) calloc(sizeof(uint32_t), 32);
	};
	~processor_t(void) {
		tfp->close();
		core->final();
		fclose(logfile);
	};
	const char *setExeFileName(const char *s) {
		exefilename = s;
		return exefilename;
	};
	void load(char *exefilename) {
		exefilename = exefilename;
		abfd = open_exe(exefilename, archname);
		load_elf(memory, abfd);
		print_mem_list(memory);
	};
	void resetCore(void) {
		p_reset = 0;	
		reset = 0;

		while(tick() <= 10) {
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
	unsigned long long tick(void) {
		static int m_clock_prev = 0;

		core->reset = reset;
		core->p_reset = p_reset;
		m_clock_prev = core->m_clock;
		core->m_clock = !core->m_clock;
		rising_edge = ((core->m_clock == 1) && (m_clock_prev == 0));

		return m_clock_count++;
	};
	void eval(void) {
		core->eval();
	};
	void dump(void) {
		tfp->dump(m_clock_count);
	};
	unsigned long long getSimTime(void) {
		return m_clock_count;
	};
	uint64_t step(void) {
		int imem_stat = 0;
		int dmem_stat = 0;
		static int got_exception = 0;
		static uint32_t epc, cause, mtval;
		uint64_t ret = -1;

		while(1) {
			tick();
			eval();
			if(core->imem_read) {
				if(imem_wait == IMEM_WAIT) {
					imem_stat = read_word(memory, core->iaddr, (uint32_t *)&core->inst);
					core->imem_valid = 1;
					imem_wait = 0;
				} else {
					core->imem_valid = 0;
					imem_wait++;
				}
			} else {
				core->imem_valid = 0;
			}
			if(core->dmem_read || core->dmem_write) {
				if(dmem_wait == DMEM_WAIT) {
					if((core->dbyteen & 0x3) == 0) {
						if(core->dmem_read) {
							dmem_stat = read_byte(memory, core->daddr, (uint8_t *)&core->rdata);
						} else if(core->dmem_write) {
							dmem_stat = write_byte(memory, core->daddr, core->wdata);
						}
					} else if((core->dbyteen & 0x3) == 1) {
						if(core->dmem_read) {
							dmem_stat = read_halfword(memory, core->daddr, (uint16_t *)&core->rdata);
						} else if(core->dmem_write) {
							dmem_stat = write_halfword(memory, core->daddr, core->wdata);
						}
					} else if((core->dbyteen & 0x3) == 2) {
						if(core->dmem_read) {
							dmem_stat = read_word(memory, core->daddr, (uint32_t *)&core->rdata);
						} else if(core->dmem_write) {
							dmem_stat = write_word(memory, core->daddr, core->wdata);
						}
					}
					core->dmem_valid = 1;
					dmem_wait = 0;
				} else {
					core->dmem_valid = 0;
					dmem_wait++;
				}
			} else {
				core->dmem_valid = 0;
			}
			if(imem_stat == -1) {
				fprintf(stderr, "Instruction Memory violation occuries at address of %08x, byteen %02x", core->iaddr, core->ibyteen);
				exit(-1);
			}
			if(dmem_stat == -1) {
				fprintf(stderr, "Data memory violation occuries at address of %08x, byteen %02x", core->daddr, core->dbyteen);
				exit(-1);
			}
			eval();
			dump();
			if(rising_edge) {
				if(core->sim_done) {
					ret = core->tohost;
				}
				if(got_exception) {
					printException(epc, cause, mtval);
					got_exception = 0;
				}
				if(core->debug_raise_exception) {
					epc = core->debug_epc;
					cause = core->debug_cause;
					mtval = core->debug_mtval;
					got_exception = 1;
				}
				if(core->debug_retire) {
					break;
				}
			}
		}
		retire_pc = core->debug_retire_pc;
		retire_inst = core->debug_retire_inst;
		fprintf(logfile, "%08x: %08x\t", retire_pc, retire_inst);
		if(disasm != NULL) {
			printDisasm(retire_pc, retire_inst);
		}
		if(rising_edge) {
			if(core->debug_mem_write) {
				int mask;
				mask = 0xffffffff >> (32-(core->debug_mem_byteen+1)*8);
				printMemWrite(core->debug_mem_adrs, core->debug_mem_data&mask);
			}
			if(core->debug_wb) {
				printRegInfo(core->debug_wb_rd, core->debug_wb_data);
			}
		}
		dumpRegs();
		fprintf(logfile, "\n");

		return ret;
	};
	void openLogFile(const char *s) {
		if(logfilename != NULL) {
			free(logfilename);
			logfilename = NULL;
		}
		logfilename = (char *)calloc(sizeof(char), strlen(s)+strlen(".log"));
		strcat(logfilename, basename(s));
		strcat(logfilename, ".log");
		puts(logfilename);
		logfile = fopen(logfilename, "w");
	};
	void openDisasm(void) {
		init_disassemble_info(&disasm_info, logfile, (fprintf_ftype) fprintf);
		disasm_info.arch = bfd_arch_riscv; 
		disasm_info.mach = bfd_mach_riscv32; 
		disasm_info.buffer_length = 0x4;
		disasm_info.buffer = instbuf;
		disasm_info.read_memory_func = buffer_read_memory;
		disassemble_init_for_target(&disasm_info);
		disasm = disassembler(bfd_arch_riscv, false, bfd_mach_riscv32, NULL);	
	};
	void printDisasm(uint32_t pc, uint32_t inst) {
		memcpy(instbuf, &inst, sizeof(uint32_t));
		disasm_info.buffer_vma = pc;
		disasm(pc, &disasm_info);
	};
	void dumpRegs(void) {
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
	void printRegInfo(uint8_t rd, uint32_t data) {
		fprintf(logfile, "%4s <- %08x", reg_strs[rd], data);
	};
	void printException(uint32_t epc, uint32_t cause, uint32_t mtval) {
		const char *str;

#define STR(e)	(#e)
		switch(cause) {
			case INSTRUCTION_ADDRESS_MISALIGNED: str = STR(INSTRUCTION_ADDRESS_MISALIGNED);
			case INSTRUCTION_ACCESS_FAULT: str = STR(INSTRUCTION_ACCESS_FAULT);
			case ILLEGAL_INSTRUCTION: str = STR(ILLEGAL_INSTRUCTION);
			case BREAKPOINT: str = STR(BREAKPOINT);
			case LOAD_ADDRESS_MISALIGNED: str = STR(LOAD_ADDRESS_MISALIGNED);
			case LOAD_ACCESS_FAULT: str = STR(LOAD_ACCESS_FAULT);
			case STORE_AMO_ADDRESS_MISALIGNED: str = STR(STORE_AMO_ADDRESS_MISALIGNED);
			case STORE_AMO_ACCESS_FAULT: str = STR(STORE_AMO_ACCESS_FAULT);
			case ENVIRONMENT_CALL_FROM_U_MODE: str = STR(ENVIRONMENT_CALL_FROM_U_MODE);
			case ENVIRONMENT_CALL_FROM_S_MODE: str = STR(ENVIRONMENT_CALL_FROM_S_MODE);
			case ENVIRONMENT_CALL_FROM_M_MODE: str = STR(ENVIRONMENT_CALL_FROM_M_MODE);
			case INSTRUCTION_PAGE_FAULT: str = STR(INSTRUCTION_PAGE_FAULT);
			case LOAD_PAGE_FAULT: str = STR(LOAD_PAGE_FAULT);
			case STORE_AMO_PAGE_FAULT: str = STR(STORE_AMO_PAGE_FAULT);
			default: str = "ILLEGAL_EXCEPTION";
		}
#undef STR

		fprintf(logfile, "%s\tepc <- %08x, mtval <- %08x\n", str, epc, mtval);
	};
	void printMemWrite(uint32_t adrs, uint32_t data) {
		fprintf(logfile, "Memory[%08x] <- %08x", adrs, data);
	};
	void flushPipeline(void) {

	};
};

int main(int argc, char **argv) {
	processor_t *proc0;
	int ret = 0;

	Verilated::commandArgs(argc, argv);
	Verilated::traceEverOn(true);

	proc0 = new processor_t();
	proc0->load(argv[1]);
	proc0->openLogFile(argv[1]);
	proc0->openDisasm();
	
	proc0->resetCore();
	while(1) {
		ret = proc0->step();
		if(ret != -1) {
			break;
		}
	}
	proc0->step(); /* flush store instruction */
	delete proc0;

	return (ret == 0x00000001) ? 0 : 1;
}
