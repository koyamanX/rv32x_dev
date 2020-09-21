#!/usr/bin/env python3
import sys

reg_bytes = 4
def gen_add_sp(v):
	print('\taddi sp, sp, {}'.format(hex(v)))
def gen_save_seq():
	for i in range(1, 32):
		print('\tsw x{}, {}(sp)'.format(str(i), hex((i-1)*reg_bytes)))
def gen_restore_seq():
	for i in range(1, 32):
		print('\tlw x{}, {}(sp)'.format(str(i), hex((i-1)*reg_bytes)))
def gen_call_c(label):
	print('\tcall {}'.format(label))
def gen_label(label):
	print('.globl {}'.format(label))
	print('{}:'.format(label))
def main(argc, argv):
	if(argc > 1):
		gen_label(argv[1])
	gen_add_sp(-32*reg_bytes)
	gen_save_seq()
	if(argc > 2):
		gen_call_c(argv[2])
	else:
		print('\t## put your codes here')
	gen_restore_seq()
	gen_add_sp(32*reg_bytes)
	return 0
if __name__ == '__main__':
	sys.exit(main(len(sys.argv), sys.argv))
