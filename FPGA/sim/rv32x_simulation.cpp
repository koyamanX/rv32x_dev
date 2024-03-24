#include <iostream>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <verilated.h>
#include "verilated_vcd_c.h"
#include "Vtopsd_sim___024root.h"
#include "Vtopsd_sim.h"
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <byteswap.h>

using namespace std;
// extern disassembler_ftype riscv_get_disassembler(bfd *);

void sim_exit(int status, void *p);
static void cpy(void *dest, void *src, size_t n)
{
	uint8_t *d, *s;

	d = (uint8_t *)dest, s = (uint8_t *)src;
	for (size_t i = 0; i < n; i += 4)
	{
		d[i + 0] = s[i + 0];
		d[i + 1] = s[i + 1];
		d[i + 2] = s[i + 2];
		d[i + 3] = s[i + 3];
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

#define REFCYC 1000 // 1000
#define SDRCYC 500	// 500
#define HALFNS 50	// 0.5ns
#define tOH 2500
#define tAC 6000
#define BLOCK_DEVICE_FILENAME "block_device.img"

uint64_t main_time = 0;
double sc_time_stamp()
{
	return main_time;
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
	unsigned int imem_wait = 0;
	unsigned int dmem_wait = 0;
	unsigned int m_clock_count = 0;
	uint32_t retire_pc = 0;
	uint32_t retire_inst = 0;
	uint32_t *regs;
	int reset = 0;
	int p_reset = 0;
	int rising_edge = 0;
	Vtopsd_sim *core;
	VerilatedVcdC *tfp;
	unsigned char instbuf[4];
	size_t buflen;
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
	uint64_t start_dump_time = 0;
	uint64_t end_dump_time = 0;
	long end_dump_inst = 0;
	int print_entry_flag = 0;
	long number_of_symbols;
	struct symbol
	{
		unsigned int addr = 0;
		unsigned int vma = 0;
		unsigned int pma = 0;
		char *name = NULL;
		int dumpflag = 0;
	} *procedure, *globalObject;
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
	int refclkCnt = 0, sdrclkCnt = SDRCYC - REFCYC, nscnt = 0;
	int sdrclk_edge = 0;

public:
	processor_t(const char *name)
	{
		procname = name;
		core = new Vtopsd_sim;
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
		if (block_device_avail)
		{
			fclose(block_device);
		}
	};

	unsigned long long tick(void)
	{
		refclkCnt++;
		sdrclkCnt++;
		nscnt++;
		rising_edge = 0;
		sdrclk_edge = 0;
		static unsigned int veritick = 0;
		if (refclkCnt == REFCYC)
		{
			rising_edge = (core->m_clock == 0);
			core->m_clock = !core->m_clock;
			refclkCnt = 0;
		}
		if (sdrclkCnt == SDRCYC)
		{
			sdrclk_edge = 1;
			core->sdrclk = !core->sdrclk;
			core->sdrclk1 = !core->sdrclk1;
			sdrclkCnt = 0;
		}
		if (nscnt == HALFNS)
		{
			veritick = veritick ? 0 : 1;
			core->rootp->topsd_sim__DOT__sdrmodel__DOT__veritick = !core->rootp->topsd_sim__DOT__sdrmodel__DOT__veritick;
			nscnt = 0;
		}
		main_time++;
		return m_clock_count++;
	};
	void resetCore(void)
	{

		core->p_reset = 0;
		core->reset = 0;
		while (tick() <= REFCYC)
		{
			eval();
			dump();
		}
		/* assert reset signal for one clock cycle */
		{
			core->p_reset = 1;
			core->reset = 1;
			while (!rising_edge)
			{
				tick();
				eval();
				dump();
			}
		}
		/* negate reset signal */
		{
			tick();
			eval();
			dump();
			core->reset = 0;
			while (!rising_edge)
			{
				tick();
				eval();
				dump();
			}
		}
	};
	void eval(void)
	{
		core->eval();
	};
	void dump(void)
	{
		if (nscnt == 0)
		{
			if (dump_vcd_flag && ((start_dump_time == 0 && end_dump_time == 0) || (start_dump_time <= main_time && end_dump_time >= main_time)))
			{
				tfp->dump(m_clock_count);
			}
		}
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
			if (rising_edge)
			{
				if (block_device_avail)
				{
					if (core->read_block)
					{
						size_t len;
						fseek(block_device, core->block_adrs, SEEK_SET);
						len = fread(buf, sizeof(uint8_t), 512, block_device);
						reverse(core->block_data, buf, 512);
						core->block_data_valid = 1;
					}
					else if (core->write_block)
					{
						fseek(block_device, core->block_adrs, SEEK_SET);
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
				// if()
			}
		}
		return ret;
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
		if (dump_vcd_flag)
		{
			dumpSetting(Darg);
			free(Darg);
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
			else if (*Darg == 't')
			{
				char *range = NULL, *bar = NULL;
				range = Darg + 2;
				bar = strstr(range, "-");
				if (bar != NULL)
				{
					*bar = '\0';
					start_dump_time = strtol(range, NULL, 0);
					end_dump_time = strtol(bar + 1, NULL, 0);
					if (start_dump_time == 0 && end_dump_time != 0)
					{
						start_dump_time = end_dump_time - 100000;
					}
					else if (start_dump_time != 0 && end_dump_time == 0)
					{
						end_dump_time = start_dump_time + 100000;
					}
					*bar = '-';
					filename = (char *)malloc(sizeof(char) * strlen(range) + strlen(fileExtension) + 1);
					strcpy(filename, range);
					vcdfilename = strcat(filename, fileExtension);
				}
			}
			else
			{
				printf("illegal dump-vcd arg\nexample:\n --dump-vcd=p,<procedure name(FUNC or NOTYPE symbol)>\n --dump-vcd=i,<start(option)>-<end(option)>\n --dump-vcd\n");
				exit(1);
			}
		}
		else
		{
			vcdfilename = (char *)malloc(sizeof(char) * 12);
			strcpy(vcdfilename, "1-10000.vcd");
		}
		tfp = new VerilatedVcdC;
		core->trace(tfp, 99);
		tfp->open(vcdfilename);
		// dump_vcd_flag = 0; // uart debug
	}
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
	proc0->parseLogOpts(argc, argv);
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

	return (ret == 0x00000001) ? 0 : ret;
}

void sim_exit(int status, void *p)
{
	env_t *env;
	env = (env_t *)p;
	tcsetattr(STDIN_FILENO, TCSANOW, env->tmio);
	if ((*(env->procs)) != NULL)
	{
		delete *(env->procs);
	}
}
