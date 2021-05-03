#!/usr/bin/env python3
import sys
import os
import math

module='plic'
irq_default=32
irq=irq_default
hart_default=2
hart=hart_default
pri_level_default=7
pri_level=pri_level_default

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
			f.write('\treg gateway_{}[32] = 0xffffffff;\n'.format(i))
			f.write('\tpending_t wire pending_{}[32];\n'.format(i))
			f.write('\twire pending_{}_w[32];\n'.format(i))
			for j in range(hart):
				f.write('\treg enable_{}_hart_{}[32] = 0;\n'.format(i, j))
		f.write('\tmem irq_priority[{}][{}] = {{0}};\n'.format(irq, int(math.log2(pri_level+1))))
		for i in range(hart):
			f.write('\treg threshold_hart_{}[{}] = 0;\n'.format(i, int(math.log2(pri_level+1))))
			f.write('\treg hart_{}_claim_num[32] = 0;\n'.format(i))
			f.write('\treg hart_{}_gateway_mask[32] = 0;\n'.format(i))
			f.write('\treg hart_{}_gateway_sel[{}] = 0;\n'.format(i, max(int(math.log2(irq//32)), 1)))
			f.write('\tproc_name hart_{}_claim(hart_{}_claim_num, hart_{}_gateway_mask, hart_{}_gateway_sel);\n'.format(i, i, i, i))
			f.write('\tproc_name hart_{}_complete(hart_{}_claim_num, hart_{}_gateway_mask, hart_{}_gateway_sel);\n'.format(i, i, i, i))
			f.write('\twire hart_{}_free;\n'.format(i))
		f.write('\tproc_name acquire_irq;\n')
		f.write('\treg irqs[32] = 0;\n')
		f.write('\treg block_num[{}] = 0;\n'.format(max(int(math.log2(irq//32)), 1)))
		f.write('\tproc_name intr_gen(block_num, irqs);\n')
		f.write('\treg claim_num[32];\n')
		f.write('\treg gateway_mask[32];\n')
		f.write('\treg gateway_sel[{}] = 0;\n'.format(max(int(math.log2(irq//32)), 1)))
		f.write('\tproc_name intr_hart(claim_num, gateway_mask, gateway_sel);\n')
		f.write('\twire irq_0;\n')
		f.write('\tirq_0 = 0;\n')
		for i in range(hart):
			f.write('\thart_{}_free = ~(hart_{}_claim || hart_{}_complete);\n'.format(i, i, i))

		for i in range(irq // 32):
			f.write('\tpending_{}_w = {{'.format(i))
			for j in range(32-1):
				f.write('pending_{}[{}].pending, '.format(i, 31-j))
			f.write('pending_{}[{}].pending}};\n'.format(i, 31-(j+1)))

		for i in range(irq // 32):
			for j in range(32):
				f.write('\tpending_{}[{}].pending = (gateway_{}[{}] & irq_{});\n'.format(i, j, i, j, (i*32)+j))

		f.write('\tproc acquire_irq {\n')
		f.write('\t\talt {\n')
		for i in range(irq // 32):
			f.write('\t\t\t(')
			for j in range(32-1):
				f.write('pending_{}[{}].pending|'.format(i, 31-j))
			f.write("pending_{}[{}].pending): intr_gen({}, ".format(i, 31-(j+1), i))
			f.write('{')
			for j in range(32-1):
				f.write('pending_{}[{}].pending,'.format(i, 31-j))
			f.write('pending_{}[{}]}});\n'.format(i, 31-(j+1)))
		f.write('\t\t}\n')
		f.write('\t}\n')


		f.write('\tproc intr_gen {\n')
		f.write('\t\twire mask[32];\n')
		f.write('\t\tif(')
		for i in range(hart-1):
			f.write('hart_{}_free|'.format(i))
		f.write('hart_{}_free) {{\n'.format(i+1))
		f.write('\t\t\talt {\n')
		for i in range(32):
			f.write("\t\t\t\tirqs[{}]: {{intr_hart(32'(6'b{:06b} << block_num), mask, block_num); mask = {:#010x};}}\n".format(i, i+1, 0x1<<i))
		f.write('\t\t\t}\n')

		f.write('\t\t} else {\n')
		f.write('\t\t\tacquire_irq();\n')
		f.write('\t\t}\n')

		f.write('\t}\n')

		f.write('\tproc intr_hart {\n')
		for i in range(hart):
			f.write('\t\twire enable_hart_{}[32];\n'.format(i))
		f.write('\t\talt {\n')
		for i in range(hart):
			f.write('\t\t\thart_{}_free && (enable_hart_{}[claim_num[{}:0]-1] && (irq_priority[claim_num[{}:0]-1] > threshold_hart_{})): {{hart_{}_claim(claim_num, gateway_mask, gateway_sel); acquire_irq.invoke();}}\n'.format(i, i, int(math.log2(irq)-1)+1, int(math.log2(irq)-1)+1, i, i))
		f.write('\t\t\telse: acquire_irq();\n')
		f.write('\t\t}\n')

		f.write('\t\tany {\n')
		for i in range(irq//32):
			f.write('\t\t\t(gateway_sel == {}): {{gateway_{} := gateway_{} & ~gateway_mask; '.format(i, i, i))
			for j in range(hart):
				f.write('enable_hart_{} = enable_{}_hart_{}; '.format(j, i, j))
			f.write('}\n')
		f.write('\t\t}\n')

		f.write('\t}\n')

		for i in range(hart):
			f.write('\tproc hart_{}_claim {{\n'.format(i))
			f.write('\t\texternal_interrupt_hart_{}();\n'.format(i))
			f.write("\t\tif(read && (addr == 32'(PLIC_CLAIM_COMPLETE_BASE_ADDR + {:#010x}))) {{hart_{}_complete(hart_{}_claim_num, hart_{}_gateway_mask, hart_{}_gateway_sel);}}\n".format(0x1000*i, i, i, i, i))
			f.write('\t}\n')

		for i in range(hart):
			f.write('\tproc hart_{}_complete {{\n'.format(i))
			f.write('\t\tfunc_self hart_{}_open_gateway();\n'.format(i))
			f.write("\t\tif(write && (addr == 32'(PLIC_CLAIM_COMPLETE_BASE_ADDR + {:#010x}))) {{if(hart_{}_claim_num == wdata) {{finish(); hart_{}_open_gateway(); hart_{}_claim_num := 0x00000000;}}}}\n".format(0x1000*i, i, i, i))
			f.write('\t}\n')

			f.write('\tfunc hart_{}_open_gateway {{\n'.format(i))
			f.write('\t\t\tany {\n')
			for j in range(irq//32):
				f.write('\t\t\t\t(hart_{}_gateway_sel == {}): gateway_{} := gateway_{} | hart_{}_gateway_mask;\n'.format(j, j, j, j, j))
			f.write('\t\t\t}\n')
			f.write('\t}\n')

		f.write('\tfunc reset {acquire_irq();}\n')

		f.write('\tfunc read {\n')
		f.write('\t\tany {\n')
		f.write("\t\t\taddr == 32'(PLIC_PRIORITY_BASE_ADDR + 0x00000000): {rdata = 0x00000000; ready();}\n")
		for i in range(1, irq):
			f.write("\t\t\taddr == 32'(PLIC_PRIORITY_BASE_ADDR + {:#010x}): {{rdata = 32'(irq_priority[{}]); ready();}}\n".format(i*4, i))
		for i in range(irq // 32):
			f.write("\t\t\taddr == 32'(PLIC_PENDING_BASE_ADDR + {:#010x}): {{rdata = pending_{}_w; ready();}}\n".format(i*4, i))
			for j in range(hart):
				f.write("\t\t\taddr == 32'(PLIC_ENABLE_BASE_ADDR + {:#010x} + {:#010x}): {{rdata = enable_{}_hart_{}; ready();}}\n".format(0x80*j, i*4, i, j))

		for i in range(hart):
			f.write("\t\t\taddr == 32'(PLIC_THRESHOLD_BASE_ADDR + {:#010x}): {{rdata = 32'(threshold_hart_{}); ready();}}\n".format(0x1000*i, i))
			f.write("\t\t\taddr == 32'(PLIC_CLAIM_COMPLETE_BASE_ADDR + {:#010x}): {{rdata = hart_{}_claim_num; ready();}}\n".format(0x1000*i, i))
		f.write('\t\t\telse: {rdata = 0x00000000; ready();}\n')
		f.write('\t\t}\n')
		f.write('\t}\n')

		f.write('\tfunc write {\n')
		f.write('\t\tany {\n')
		f.write("\t\t\taddr == 32'(PLIC_PRIORITY_BASE_ADDR + 0x00000000): {; ready();}\n")
		for i in range(1, irq):
			f.write("\t\t\taddr == 32'(PLIC_PRIORITY_BASE_ADDR + {:#010x}): {{irq_priority[{}] := wdata[{}:0]; ready();}}\n".format(i*4, i, int(math.log2(pri_level+1))-1))
		for i in range(irq // 32):
			f.write("\t\t\taddr == 32'(PLIC_PENDING_BASE_ADDR + {:#010x}): {{; ready();}}\n".format(i*4, i))
			for j in range(hart):
				f.write("\t\t\taddr == 32'(PLIC_ENABLE_BASE_ADDR + {:#010x} + {:#010x}): {{enable_{}_hart_{} := wdata; ready();}}\n".format(0x80*j, i*4, i, j))
		for i in range(hart):
			f.write("\t\t\taddr == 32'(PLIC_THRESHOLD_BASE_ADDR + {:#010x}): {{threshold_hart_{} := wdata[{}:0]; ready();}}\n".format(0x1000*i, i, int(math.log2(pri_level+1))-1))
			f.write("\t\t\taddr == 32'(PLIC_CLAIM_COMPLETE_BASE_ADDR + {:#010x}): {{; ready();}}\n".format(0x1000*i, i))
		f.write('\t\t\telse: {ready();}\n')
		f.write('\t\t}\n')
		f.write('\t}\n')

		f.write('}\n')


def main(argc, argv):
	gen_header(module+'.h')
	gen_nsl(module+'.nsl')

if __name__ == '__main__':
	main(len(sys.argv), sys.argv)
