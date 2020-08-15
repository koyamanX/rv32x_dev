#define PACKAGE "bfd"
#include <bfd.h>
#include <dis-asm.h>
#include "elfloader/elfloader.h"
#include <float.h>
#include <stdlib.h>
#include <verilated.h>
#include "verilated_vcd_c.h"
#include "Vrv32x_simulation.h"

/* TODO: Re-write the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */
/* TODO: clean up the code below */

Vrv32x_simulation *top; 
VerilatedVcdC *tfp;
unsigned int main_time = 0;
const char *vcdfile = "rv32x_simulation.vcd";
const char *arch = "elf32-littleriscv";
const char *exefile = "a.out";
uint32_t regs[33];
FILE *logfile;
disassembler_ftype disasm;
int m_clock_prev = 0;
unsigned char instbuf[4];
uint32_t insn_size;	
struct disassemble_info disasm_info = {};
memlist_t *m;
int imem_wait = 0;
int dmem_wait = 0;
unsigned int retire_pc;
unsigned int retire_inst;

double sc_time_stamp(void);
void init(void);
void parseopt(int argc, char **argv);
void regdump(void); 
void printregs(void); 
void finish(int stat);

const char *reg_strs[] = {
	"zero",
	"ra",
	"sp",
	"gp",
	"tp",
	"t0",
	"t1",
	"t2",
	"s0",
	"s1",
	"a0",
	"a1",
	"a2",
	"a3",
	"a4",
	"a5",
	"a6",
	"a7",
	"s2",
	"s3",
	"s4",
	"s5",
	"s6",
	"s7",
	"s8",
	"s9",
	"s10",
	"s11",
	"t3",
	"t4",
	"t5",
	"t6"
};

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

#define E2S(e)	(#e)

const char *exception_str(uint32_t cause) {
	switch(cause) {
		case INSTRUCTION_ADDRESS_MISALIGNED: return E2S(INSTRUCTION_ADDRESS_MISALIGNED);
		case INSTRUCTION_ACCESS_FAULT: return E2S(INSTRUCTION_ACCESS_FAULT);
		case ILLEGAL_INSTRUCTION: return E2S(ILLEGAL_INSTRUCTION);
		case BREAKPOINT: return E2S(BREAKPOINT);
		case LOAD_ADDRESS_MISALIGNED: return E2S(LOAD_ADDRESS_MISALIGNED);
		case LOAD_ACCESS_FAULT: return E2S(LOAD_ACCESS_FAULT);
		case STORE_AMO_ADDRESS_MISALIGNED: return E2S(STORE_AMO_ADDRESS_MISALIGNED);
		case STORE_AMO_ACCESS_FAULT: return E2S(STORE_AMO_ACCESS_FAULT);
		case ENVIRONMENT_CALL_FROM_U_MODE: return E2S(ENVIRONMENT_CALL_FROM_U_MODE);
		case ENVIRONMENT_CALL_FROM_S_MODE: return E2S(ENVIRONMENT_CALL_FROM_S_MODE);
		case ENVIRONMENT_CALL_FROM_M_MODE: return E2S(ENVIRONMENT_CALL_FROM_M_MODE);
		case INSTRUCTION_PAGE_FAULT: return E2S(INSTRUCTION_PAGE_FAULT);
		case LOAD_PAGE_FAULT: return E2S(LOAD_PAGE_FAULT);
		case STORE_AMO_PAGE_FAULT: return E2S(STORE_AMO_PAGE_FAULT);
		default: return "ILLEGAL_EXCEPTION";
	}
}

int main(int argc, char **argv)
{
	Verilated::commandArgs(argc, argv);
	Verilated::traceEverOn(true);
	char logfilename[128] = {0};
	int i;
	uint32_t mtval, epc, cause, got_exception = 0;
	init();

	if(argc > 1) {
	   exefile = argv[1]; 
	}
	//WData packed_data[4];
	//WData packed_inst[4];
	m = init_mem_list();
	bfd *abfd = open_exe(exefile, arch);
	load_elf(m, abfd);
	print_mem_list(m);

	assert(strlen(exefile) < 128-4);
	strcat(logfilename, basename(exefile));
	strcat(logfilename, ".log");
	logfile = fopen(logfilename, "w");
	assert(logfile != NULL);
	fprintf(stderr, "open %s for logging\n", logfilename);

	init_disassemble_info(&disasm_info, logfile, (fprintf_ftype) fprintf);
	disasm_info.arch = bfd_arch_riscv; 
	disasm_info.mach = bfd_mach_riscv32; 
	disasm_info.buffer_length = 0x4;
	//disasm_info.section = section;
	disasm_info.buffer = instbuf;
	disasm_info.read_memory_func = buffer_read_memory;
	disassemble_init_for_target(&disasm_info);
	disasm = disassembler(bfd_arch_riscv, false, bfd_mach_riscv32, NULL);	
	if(disasm == NULL) {
		fprintf(stderr, "cannot find disassembler\n");
		exit(1);
	}


	fprintf(stderr, "open %s \n", vcdfile);
	top->trace(tfp, 99);
	tfp->open(vcdfile);

	top->p_reset = 0;
	top->m_clock = 0;
	while(!Verilated::gotFinish()) {
		if((main_time % 2) == 0) {
			m_clock_prev = top->m_clock;
			top->m_clock = !top->m_clock;
		}
		if(main_time > 10) {
			top->p_reset = 1;
		}
		if((main_time == 10) || (main_time == 12)) {
			top->reset = 1;
		} else {
			top->reset = 0;
		}

		/* top->eval() to get current imem_read, dmem_read, dmem_write signal 
			eval() to see current values
		*/	
		top->eval(); 
		if(top->sim_done && (top->m_clock == 1 && m_clock_prev == 0)) {
			int ret = 1;

			if(top->tohost == 0x00000001) {
				ret = 0;
			}
			finish(ret);
		}
		if(top->debug_retire && (top->m_clock == 1 && m_clock_prev == 0)) {
			retire_pc = top->debug_retire_pc;
			retire_inst = top->debug_retire_inst;
			fprintf(logfile, "0x%016x (0x%08x)\t", top->debug_retire_pc, top->debug_retire_inst);
			memcpy(instbuf, &top->debug_retire_inst, sizeof(int));
			disasm_info.buffer_vma = top->debug_retire_pc;
			insn_size = disasm(top->debug_retire_pc, &disasm_info);
			if(top->debug_wb) {
				fprintf(logfile, "\t\t\t\t%s <- %08x", reg_strs[top->debug_wb_rd&0x1f], top->debug_wb_data);
			}
			if(top->debug_mem_write) {
				int mask;
				mask = 0xffffffff >> (32-(top->debug_mem_byteen+1)*8);
				fprintf(logfile, "\tmemory[%08x] <- %08x", top->debug_mem_adrs, top->debug_mem_data&mask);
			}
			fprintf(logfile, "\n");
		}
		if(top->imem_read) {
			if(imem_wait == 5) {
			/*
				for(i = 0; i < 4; i++)
					packed_inst[i] = 0;
				for(i = 0; i < 4; i++) {
					packed_inst[i] = read_word(m, top->iaddr+(i*4));
				}
				memcpy(top->inst, packed_inst, sizeof(WData)*4);
				//top->inst = packed_inst;                   
			*/
				top->inst = read_word(m, top->iaddr);
				top->imem_valid = 1;
				imem_wait = 0;
			} else {
				top->imem_valid = 0;
				imem_wait++;
			}
		} else {
			top->imem_valid = 0;
		}
		if(top->dmem_read) {
			if(dmem_wait == 5) {
			/*
				for(i = 0; i < 4; i++)
					packed_data[i] = 0;
				for(i = 0; i < 4; i++) {
					packed_data[i] = read_word(m, top->daddr+(i*4));
				}
				memcpy(top->rdata, packed_data, sizeof(WData)*4);
			*/
				if((top->dbyteen & 0x3) == 0) {
					top->rdata = read_byte(m, top->daddr);
				} else if((top->dbyteen & 0x3) == 1) {
					top->rdata = read_halfword(m, top->daddr);
				} else if((top->dbyteen & 0x3) == 2) {
					top->rdata = read_word(m, top->daddr);
				}
				top->dmem_valid = 1;
				dmem_wait = 0;
			} else {
				top->dmem_valid = 0;
				dmem_wait++;
			}
		} else if(top->dmem_write) {
			if(dmem_wait == 5) {
			/*
				for(i = 0; i < 4; i++)
					packed_data[i] = 0;

				memcpy(packed_data, top->wdata, sizeof(WData)*4);

				for(i = 0; i < 4; i++) {
					write_word(m, top->daddr+(i*4), packed_data[i]);
				}
			*/
				if((top->dbyteen & 0x3) == 0) {
					write_byte(m, top->daddr, top->wdata);
				} else if((top->dbyteen & 0x3) == 1) {
					write_halfword(m, top->daddr, top->wdata);
				} else if((top->dbyteen & 0x3) == 2) {
					write_word(m, top->daddr, top->wdata);
				}
				top->dmem_valid = 1;
				dmem_wait = 0;
			} else {
				top->dmem_valid = 0;
				dmem_wait++;
			}
		} else {
			top->dmem_valid = 0;
		}
		if(got_exception && (top->m_clock && !m_clock_prev)) {
			fprintf(logfile, "%s (%08x): mtval %08x\n", exception_str(top->debug_cause), top->debug_epc, top->debug_mtval);
			got_exception = 0;
		}
		if(top->debug_raise_exception && (top->m_clock && !m_clock_prev)) {
			cause = top->debug_cause;
			epc = top->debug_epc;
			mtval = top->debug_mtval;
			got_exception = 1;
		}
		/* Increment simulation time */
		main_time+=2;
		top->eval();			/* eval again to feed value from memory */
		tfp->dump(main_time);
	}
	finish(EXIT_SUCCESS);
	return 0;
}

double sc_time_stamp() {
	return main_time;
}
void init(void) {
	top = new Vrv32x_simulation;
	tfp = new VerilatedVcdC;
}
void parseopt(int argc, char **argv) {
	/* TODO: Implement */
	return;
}
void regdump(void) {
	regs[0] = top->debug_x0;	
	regs[1] = top->debug_x1;	
	regs[2] = top->debug_x2;	
	regs[3] = top->debug_x3;	
	regs[4] = top->debug_x4;	
	regs[5] = top->debug_x5;	
	regs[6] = top->debug_x6;	
	regs[7] = top->debug_x7;	
	regs[8] = top->debug_x8;	
	regs[9] = top->debug_x9;	
	regs[10] = top->debug_x10;	
	regs[11] = top->debug_x11;	
	regs[12] = top->debug_x12;	
	regs[13] = top->debug_x13;	
	regs[14] = top->debug_x14;	
	regs[15] = top->debug_x15;	
	regs[16] = top->debug_x16;	
	regs[17] = top->debug_x17;	
	regs[18] = top->debug_x18;	
	regs[19] = top->debug_x19;	
	regs[20] = top->debug_x20;	
	regs[21] = top->debug_x21;	
	regs[22] = top->debug_x22;	
	regs[23] = top->debug_x23;	
	regs[24] = top->debug_x24;	
	regs[25] = top->debug_x25;	
	regs[26] = top->debug_x26;	
	regs[27] = top->debug_x27;	
	regs[28] = top->debug_x28;	
	regs[29] = top->debug_x29;	
	regs[30] = top->debug_x30;	
	regs[31] = top->debug_x31;	
	regs[32] = top->debug_pc;
}
void printregs(void) {
	char str[32];
	int i;

	fprintf(stderr, "pc: %x\n", regs[32]);
	for(i = 1; i < 32; i++) {
		sprintf(str, "x%d", i);
		fprintf(stderr, "%s:%x\t", reg_strs[i], regs[i]);
		if((i % 4 == 0))
			fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");
}
void finish(int stat) { 
	int ret = stat;
	int retire_cnt = 0;
	uint32_t mtval, epc, cause, got_exception = 0;

	/* Some cycles for Write back */
	while(1) {
		top->eval();
		if((main_time % 2) == 0) {
			m_clock_prev = top->m_clock;
			top->m_clock = !top->m_clock;
		}
		if(top->dmem_read) {
			/*
				for(i = 0; i < 4; i++)
					packed_data[i] = 0;
				for(i = 0; i < 4; i++) {
					packed_data[i] = read_word(m, top->daddr+(i*4));
				}
				memcpy(top->rdata, packed_data, sizeof(WData)*4);
			*/
			if((top->dbyteen & 0x3) == 0) {
				top->rdata = read_byte(m, top->daddr);
			} else if((top->dbyteen & 0x3) == 1) {
				top->rdata = read_halfword(m, top->daddr);
			} else if((top->dbyteen & 0x3) == 2) {
				top->rdata = read_word(m, top->daddr);
			}
			top->dmem_valid = 1;
		} else if(top->dmem_write) {
		/*
			for(i = 0; i < 4; i++)
				packed_data[i] = 0;

			memcpy(packed_data, top->wdata, sizeof(WData)*4);

			for(i = 0; i < 4; i++) {
				write_word(m, top->daddr+(i*4), packed_data[i]);
			}
		*/
			if((top->dbyteen & 0x3) == 0) {
				write_byte(m, top->daddr, top->wdata);
			} else if((top->dbyteen & 0x3) == 1) {
				write_halfword(m, top->daddr, top->wdata);
			} else if((top->dbyteen & 0x3) == 2) {
				write_word(m, top->daddr, top->wdata);
			}
			top->dmem_valid = 1;
		} else {
			top->dmem_valid = 0;
		}

		if(top->debug_retire && (top->m_clock && !m_clock_prev)) {
			fprintf(logfile, "0x%016x (0x%08x)\t", top->debug_retire_pc, top->debug_retire_inst);
			disasm_info.buffer_vma = top->debug_retire_pc;
			memcpy(instbuf, &top->debug_retire_inst, sizeof(int));
			insn_size = disasm(top->debug_retire_pc, &disasm_info);
			if(top->debug_wb) {
				fprintf(logfile, "\t\t\t\t%s <- %08x", reg_strs[top->debug_wb_rd&0x1f], top->debug_wb_data);
			}
			if(top->debug_mem_write) {
				int mask;
				mask = 0xffffffff >> (32-(top->debug_mem_byteen+1)*8);
				fprintf(logfile, "\tmemory[%08x] <- %08x", top->debug_mem_adrs, top->debug_mem_data&mask);
			}
			fprintf(logfile, "\n");
			if(retire_cnt == 1) {
				break;
			}
			retire_cnt++;
		}
		if(got_exception && (top->m_clock && !m_clock_prev)) {
			fprintf(logfile, "%s (%08x): mtval %08x\n", exception_str(top->debug_cause), top->debug_epc, top->debug_mtval);
			got_exception = 0;
		}
		if(top->debug_raise_exception && (top->m_clock && !m_clock_prev)) {
			cause = top->debug_cause;
			epc = top->debug_epc;
			mtval = top->debug_mtval;
			got_exception = 1;
		}
		main_time+=2;
		top->eval();			/* eval again to feed value from memory */
		tfp->dump(main_time);
	}
#ifdef DEBUG
	regdump();
	printregs();
#endif
	tfp->close(); 
	top->final(); 
	exit(ret); 
} 
