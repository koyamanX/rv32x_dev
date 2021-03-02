#!/usr/bin/env python3
import sys
import os
import math

module='plic'
irq_default=32
irq=min(irq_default, 32)
hart_default=2
hart=min(hart_default, 2)
pri_level_default=7
pri_level=min(pri_level_default, 7)

macro_const = [
	['PLIC_BASE_ADDR', 0x0c000000],
	['PLIC_PRIORITY_BASE_ADDR', 0x0c000000],
	['PLIC_PENDING_BASE_ADDR', 0x0c001000],
	['PLIC_ENABLE_BASE_ADDR', 0x0c002000],
	['PLIC_THRESHOLD_BASE_ADDR', 0x0c200000],
	['PLIC_CLAIM_COMPLETE_BASE_ADDR', 0x0c200004],
	['PLIC_SIZE', 0x10000000],
]

def gen_header(filename):
	with open(filename, "w") as f:
		f.write('#ifndef {}_H\n'.format(module.upper()))
		f.write('#define {}_H\n'.format(module.upper()))
		for i in macro_const:
			f.write('#define {} {:#010x}\n'.format(i[0], i[1]))
		f.write('declare {} {{\n'.format(module))
		f.write('\tinput addr[32];\n')
		f.write('\tinput wdata[32];\n')
		f.write('\toutput rdata[32];\n')
		f.write('\tfunc_in read(addr);\n')
		f.write('\tfunc_in write(addr, wdata);\n')
		f.write('\tfunc_in reset();\n')
		f.write('\tfunc_out ready();\n')
		f.write('\tfunc_out store_access_fault();\n')
		f.write('\tfunc_out load_access_fault();\n')
		for i in range(hart):
			f.write('\tfunc_out external_interrupt_hart_{}();\n'.format(i))
		for i in range(1, irq):
			f.write('\tfunc_in irq_{};\n'.format(i))
		f.write('}\n')
		f.write('struct {} {{\n\t{}[1];\n}};\n'.format('pending_t', 'pending'))
		f.write('struct {} {{\n\t{}[1];\n}};\n'.format('gateway_t', 'gateway'))
		f.write('#endif\n')


def gen_nsl(filename):
	with open(filename, "w") as f:
		f.write('#include "{}"\n'.format(os.path.splitext(filename)[0]+'.h'))
		f.write('module {} {{\n'.format(module))
		for i in range(irq // 32):
			f.write('\tgateway_t reg gateway_{}[32] = 0xffffffff;\n'.format(i))
			f.write('\tpending_t reg pending_{}[32] = 0;\n'.format(i))
			f.write('\twire pending_{}_w[32];\n'.format(i))
			for j in range(hart):
				f.write('\treg enable_{}_hart_{}[32] = 0;\n'.format(i, j))
		for i in range(irq):
			f.write('\treg irq_{}_priority[{}] = 0;\n'.format(i, int(math.log2(pri_level+1))))
		for i in range(hart):
			f.write('\treg threshold_hart_{}[{}] = 0;\n'.format(i, int(math.log2(pri_level+1))))
			f.write('\treg claim_complete_hart_{}[32] = 0;\n'.format(i))
			f.write('\treg hart_{}_inservice = 0;\n'.format(i))
			f.write('\treg hart_{}_inservice_claim[32] = 0;\n'.format(i))
			f.write('\treg hart_{}_in_intreq = 0;\n'.format(i))
			f.write('\tproc_name hart_{};\n'.format(i))
		f.write('\twire irq_0;\n\tirq_0 = 0;\n')

		for i in range(irq // 32):
			f.write('\tpending_{}_w = {{'.format(i))
			for j in range(32-1):
				f.write('pending_{}[{}].pending, '.format(i, (i*32)+j))
			f.write('pending_{}[{}].pending}};\n'.format(i, (i*32)+j+1))

		for i in range(irq // 32):
			for j in range(32):
				f.write('\tpending_{}[{}].pending := ((gateway_{}[{}].gateway & irq_{}) | pending_{}[{}].pending);\n'.format(i, j, i, j, j, i, j))

		for i in range(hart):
			f.write('\tproc hart_{} {{\n'.format(i))
			f.write('\t\tif(!hart_{}_inservice && !hart_{}_in_intreq) {{\n'.format(i, i))
			f.write('\t\t\talt {\n')
			for j in range(irq // 32):
				for k in range(32):
					f.write('\t\t\t\t(enable_{}_hart_{}[{}] && pending_{}[{}].pending && (irq_{}_priority > threshold_hart_{})): {{hart_{}_in_intreq := 1; hart_{}_inservice_claim := {}; gateway_{}[{}].gateway := 0;}}\n'.format(j, i, k, j, k, k, i, i, i, j*32+k, j, k))
			f.write('\t\t\t}\n')
			f.write('\t\t}\n')

			f.write('\t\tif(!hart_{}_in_intreq && hart_{}_inservice) {{\n'.format(i, i))
			f.write('\t\t\thart_{}();\n'.format(((hart - i) // hart)))
			f.write('\t\t}\n')
			f.write('\t\tif(hart_{}_in_intreq && !hart_{}_inservice) {{\n'.format(i, i))
			f.write('\t\t\texternal_interrupt_hart_{}();\n'.format(i))
			f.write('\t\t}\n')
			f.write('\t}\n')

		f.write('\tfunc reset {hart_0();}\n')

		f.write('\tfunc read {\n')
		f.write('\t\tany {\n')
		f.write("\t\t\taddr == 32'(PLIC_PRIORITY_BASE_ADDR + 0x00000000): {rdata = 0x00000000; ready();}\n")
		for i in range(1, irq):
			f.write("\t\t\taddr == 32'(PLIC_PRIORITY_BASE_ADDR + {:#010x}): {{rdata = 32'(irq_{}_priority); ready();}}\n".format(i*4, i))
		for i in range(irq // 32):
			f.write("\t\t\taddr == 32'(PLIC_PENDING_BASE_ADDR + {:#010x}): {{rdata = pending_{}_w; ready();}}\n".format(i*4, i))
			for j in range(hart):
				f.write("\t\t\taddr == 32'(PLIC_ENABLE_BASE_ADDR + {:#010x} + {:#010x}): {{rdata = enable_{}_hart_{}; ready();}}\n".format(0x80*j, i*4, i, j))

		for i in range(hart):
			f.write("\t\t\taddr == 32'(PLIC_THRESHOLD_BASE_ADDR + {:#010x}): {{rdata = 32'(threshold_hart_{}); ready();}}\n".format(0x1000*i, i))
			f.write("\t\t\taddr == 32'(PLIC_CLAIM_COMPLETE_BASE_ADDR + {:#010x}): {{rdata = claim_complete_hart_{}; ready(); if(hart_{}_in_intreq) {{hart_{}_in_intreq := 0; hart_{}_inservice := 1;}}}}\n".format(0x1000*i, i, i, i, i))
		f.write('\t\t\telse: load_access_fault();\n')
		f.write('\t\t}\n')
		f.write('\t}\n')

		f.write('\tfunc write {\n')
		f.write('\t\tany {\n')
		f.write("\t\t\taddr == 32'(PLIC_PRIORITY_BASE_ADDR + 0x00000000): {; ready();}\n")
		for i in range(1, irq):
			f.write("\t\t\taddr == 32'(PLIC_PRIORITY_BASE_ADDR + {:#010x}): {{irq_{}_priority := wdata[{}:0]; ready();}}\n".format(i*4, i, int(math.log2(pri_level+1))-1))
		for i in range(irq // 32):
			f.write("\t\t\taddr == 32'(PLIC_PENDING_BASE_ADDR + {:#010x}): {{; ready();}}\n".format(i*4, i))
			for j in range(hart):
				f.write("\t\t\taddr == 32'(PLIC_ENABLE_BASE_ADDR + {:#010x} + {:#010x}): {{enable_{}_hart_{} := wdata; ready();}}\n".format(0x80*j, i*4, i, j))
		for i in range(hart):
			f.write("\t\t\taddr == 32'(PLIC_THRESHOLD_BASE_ADDR + {:#010x}): {{threshold_hart_{} := wdata[{}:0]; ready();}}\n".format(0x1000*i, i, int(math.log2(pri_level+1))-1))
			f.write("\t\t\taddr == 32'(PLIC_CLAIM_COMPLETE_BASE_ADDR + {:#010x}): {{claim_complete_hart_{} := wdata; ready(); if(hart_{}_inservice_claim == wdata) hart_{}_inservice := 0;}}\n".format(0x1000*i, i, i, i))
		f.write('\t\t\telse: store_access_fault();\n')
		f.write('\t\t}\n')
		f.write('\t}\n')

		f.write('}\n')


def main(argc, argv):
	gen_header(module+'.h')
	gen_nsl(module+'.nsl')

if __name__ == '__main__':
	main(len(sys.argv), sys.argv)
