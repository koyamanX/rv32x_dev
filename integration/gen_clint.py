#!/usr/bin/env python3
import sys
import os

module='clint'
size=1
macro_const = [
	['CLINT_BASE_ADDR', 0x02000000],
	['CLINT_MSIP_ADDR_OFFSET', 0x0000],
	['CLINT_MTIMECMP_ADDR_OFFSET', 0x4000],
	['CLINT_MTIME_ADDR_OFFSET', 0xbff8],
	['CLINT_SIZE', 0x0000c000]
]
def gen_header(filename):
	with open(filename, "w") as f:
		f.write('#ifndef {}_H\n'.format(module.upper()))
		f.write('#define {}_H\n'.format(module.upper()))
		for i in macro_const:
			f.write('#define {} {:#06x}\n'.format(i[0], i[1]))
		f.write('declare {} {{\n'.format(module))
		f.write('\tinput addr[16];\n')
		f.write('\tinput wdata[32];\n')
		f.write('\toutput rdata[32];\n')
		f.write('\tfunc_in read(addr);\n')
		f.write('\tfunc_in write(addr, wdata);\n')
		f.write('\tfunc_out ready();\n')
		f.write('\tfunc_out store_access_fault();\n')
		f.write('\tfunc_out load_access_fault();\n')
		for i in range(size):
			f.write('\tfunc_out software_interrupt_hart{}();\n'.format(i))
			f.write('\tfunc_out timer_interrupt_hart{}();\n'.format(i))
		f.write('}\n')
		f.write('#endif\n')

def gen_nsl(filename):
	with open(filename, "w") as f:
		f.write('#include "{}"\n'.format(os.path.splitext(filename)[0]+'.h'))
		f.write('module {} {{\n'.format(module))
		for i in range(size):
			f.write('\treg mtimecmp_{}_hi[32] = 0;\n'.format(str(i)))
			f.write('\treg mtimecmp_{}_lo[32] = 0;\n'.format(str(i)))
			f.write('\twire mtimecmp_{}[64];\n'.format(str(i)))
			f.write('\treg msip_{} = 0;\n'.format(str(i)))
		f.write('\treg mtime_hi[32] = 0;\n')
		f.write('\treg mtime_lo[32] = 0;\n')
		f.write('\twire mtime[64];\n')
		f.write('\twire mtime_n[64];\n')
		f.write('\tfunc read {\n')
		f.write('\t\tany {\n')
		for i in range(size):
			f.write("\t\t\taddr == 16'(CLINT_MSIP_ADDR_OFFSET + {:#06x}): {{rdata = 32'(msip_{}); ready();}}\n".format(i*4, str(i)))
			f.write("\t\t\taddr == 16'(CLINT_MTIMECMP_ADDR_OFFSET + {:#06x} + 0x0000): {{rdata = mtimecmp_{}_lo; ready();}}\n".format(i*4, str(i)))
			f.write("\t\t\taddr == 16'(CLINT_MTIMECMP_ADDR_OFFSET + {:#06x} + 0x0004): {{rdata = mtimecmp_{}_hi; ready();}}\n".format(i*4, str(i)))
		f.write("\t\t\taddr == 16'(CLINT_MTIME_ADDR_OFFSET + 0x0000): {{rdata = mtime_lo; ready();}}\n")
		f.write("\t\t\taddr == 16'(CLINT_MTIME_ADDR_OFFSET + 0x0004): {{rdata = mtime_hi; ready();}}\n")
		f.write('\t\t\telse: load_access_fault();\n')
		f.write('\t\t}\n')
		f.write('\t}\n')

		f.write('\tfunc write {\n')
		f.write('\t\tany {\n')
		for i in range(size):
			f.write("\t\t\taddr == 16'(CLINT_MSIP_ADDR_OFFSET + {:#06x}): {{msip_{} := wdata[0]; ready();}}\n".format(i*4, str(i)))
			f.write("\t\t\taddr == 16'(CLINT_MTIMECMP_ADDR_OFFSET + {:06x} + 0x0000): {{mtimecmp_{}_lo := wdata; ready();}}\n".format(i*4, str(i)))
			f.write("\t\t\taddr == 16'(CLINT_MTIMECMP_ADDR_OFFSET + {:06x} + 0x0004): {{mtimecmp_{}_hi := wdata; ready();}}\n".format(i*4, str(i)))
		f.write("\t\t\taddr == 16'(CLINT_MTIME_ADDR_OFFSET + 0x0000): {{mtime_lo := wdata; ready();}}\n")
		f.write("\t\t\taddr == 16'(CLINT_MTIME_ADDR_OFFSET + 0x0004): {{mtime_hi := wdata; ready();}}\n")
		f.write('\t\t\telse: store_access_fault();\n')
		f.write('\t\t}\n')
		f.write('\t}\n')



		f.write('\tmtime = {mtime_hi, mtime_lo};\n')
		f.write('\tmtime_n = mtime + 0x0000000000000001;\n')
		f.write('\tif(!write) {mtime_hi := mtime_n[63:32]; mtime_lo := mtime_n[31:0];}\n')
		for i in range(size):
			f.write('\tmtimecmp_{} = {{mtimecmp_{}_hi, mtimecmp_{}_lo}};\n'.format(str(i), str(i), str(i)))
			f.write('\tif(mtime >= mtimecmp_{}) timer_interrupt_hart{}();\n'.format(str(i), str(i)))
			f.write('\tif(msip_{}) software_interrupt_hart{}();\n'.format(str(i), str(i)))
		f.write('}\n')
		
def main(argc, argv):
	gen_header('clint.h')
	gen_nsl('clint.nsl')

if __name__ == '__main__':
	main(len(sys.argv), sys.argv)
