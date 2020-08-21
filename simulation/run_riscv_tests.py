#!/usr/bin/env python3

import glob
import os
import sys
import subprocess
import shutil

simulator='./rv32x_simulation'
riscv_tests_directory = '../software/riscv-tests/isa/'
simulation_log_directory = './logs/'
simulation_failed_directory = './failed/'
targets = [
	'rv32ui-p-*',
	'rv32um-p-*',
	'rv32mi-p-*',
]
targets_should_be_failed = [
	'rv32mi-p-breakpoint',
	'rv32mi-p-ma_fetch',
	'rv32mi-p-ma_addr',
]
def gen_tests_list():
	tests_list = list()
	for i in targets:
		for j in glob.iglob(riscv_tests_directory+i):
			if j[-5:] != '.dump':
				tests_list.append(os.path.basename(j))
	return tests_list
def gen_tests_assertion(tests_list):
	tests_assertion = ['should_be_passed' for _ in tests_list]	
	for i in targets_should_be_failed:
		tests_assertion[tests_list.index(i)] = 'should_be_failed'
	return tests_assertion
def run_tests(tests_list, tests_assertion):
	for t, a in zip(tests_list, tests_assertion):
		try:
			subprocess.run([simulator, riscv_tests_directory+t], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True, timeout=5)
		except subprocess.CalledProcessError as err:
			shutil.move(simulator+'.vcd', simulation_failed_directory+t)
			if(a == 'should_be_passed'):
				print('{:20}\t{:20}  and it\'s FAILED\tNG'.format(t, a))
				return 1
			else:
				print('{:20}\t{:20}  and it\'s FAILED\tOK'.format(t, a))
		except subprocess.TimeoutExpired:
			shutil.move(simulator+'.vcd', simulation_failed_directory+t)
			print('{:20}\t{:20}  and it\'s TIMEOUT\tNG'.format(t, a))
			return 1
		except:
			shutil.move(simulator+'.vcd', simulation_failed_directory+t)
			print('unexpected error')
			sys.exit(1)
		finally:
			shutil.move(t+'.log', simulation_log_directory)
		if(a == 'should_be_passed'):
			print('{:20}\t{:20}  and it\'s PASSED\tOK'.format(t, a))
		elif(a == 'should_not_passed'):
			shutil.move(simulator+'.vcd', simulation_failed_directory+t)
			print('{:20}\t{:20}  and it\'s PASSED\tNG'.format(t, a))
def main():
	if(os.path.exists(simulation_failed_directory)):
		shutil.rmtree(simulation_failed_directory)
	if(os.path.exists(simulation_log_directory)):
		shutil.rmtree(simulation_log_directory)
	os.makedirs(simulation_failed_directory)
	os.makedirs(simulation_log_directory)
	tests_list = gen_tests_list()
	tests_assertion = gen_tests_assertion(tests_list)
	exit_code = run_tests(tests_list, tests_assertion)

	return exit_code
if __name__ == '__main__':
	sys.exit(main())
