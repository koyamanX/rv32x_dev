#!/usr/bin/env python3

import glob
import os
import sys
import subprocess
import shutil

simulator='./rv32x_simulation'
riscv_tests_directory = '../software/riscv-tests/isa/'
simulation_log_directory = './logs/'
targets = [
	'rv32ui-p-*',
	'rv32um-p-*',
	'rv32mi-p-*',
	'rv32ua-p-*',
	'rv32si-p-*',
]
targets_should_be_failed = [
	'rv32mi-p-breakpoint',
	'rv32ua-p-amoadd_w',
	'rv32ua-p-amoand_w',
	'rv32ua-p-amomaxu_w',
	'rv32ua-p-amomax_w',
	'rv32ua-p-amominu_w',
	'rv32ua-p-amomin_w',
	'rv32ua-p-amoor_w',
	'rv32ua-p-amoswap_w',
	'rv32ua-p-amoxor_w',
	'rv32ua-p-lrsc',
	'rv32si-p-csr',
	'rv32si-p-dirty',
	'rv32si-p-ma_fetch',
	'rv32si-p-sbreak',
	'rv32si-p-scall',
	'rv32si-p-wfi',
]
failed=0
failed_tests = []
timeout=0
passed=0
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
	global passed, failed, timeout
	for t, a in zip(tests_list, tests_assertion):
		try:
			subprocess.run([simulator, riscv_tests_directory+t, '--print-inst-trace', '--print-exception', '--print-disasm'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True, timeout=8)
		except subprocess.CalledProcessError as err:
			if(a == 'should_be_passed'):
				print('{:20}\t{:20}  and it\'s FAILED\tNG'.format(t, a))
				failed_tests.append(t)
				failed+=1
			else:
				print('{:20}\t{:20}  and it\'s FAILED\tOK'.format(t, a))
				failed_tests.append(t)
				failed+=1
			continue
		except subprocess.TimeoutExpired:
			print('{:20}\t{:20}  and it\'s TIMEOUT\tNG'.format(t, a))
			failed_tests.append(t)
			timeout+=1
			failed+=1
			continue
		except:
			print('unexpected error')
			sys.exit(1)
		finally:
			shutil.move(t+'.log', simulation_log_directory)
		if(a == 'should_be_passed'):
			print('{:20}\t{:20}  and it\'s PASSED\tOK'.format(t, a))
			passed+=1
		elif(a == 'should_not_passed'):
			print('{:20}\t{:20}  and it\'s PASSED\tNG'.format(t, a))
			failed_tests.append(t)
			failed+=1
def main():
	md_opt = 0
	if len(sys.argv) > 1 and sys.argv[1] == '--print-markdown':
		md_opt = 1
	if(os.path.exists(simulation_log_directory)):
		shutil.rmtree(simulation_log_directory)
	os.makedirs(simulation_log_directory)
	tests_list = gen_tests_list()
	tests_assertion = gen_tests_assertion(tests_list)
	if md_opt == 1:
		print('```')
	run_tests(tests_list, tests_assertion)
	exit_code = failed

	if md_opt == 1:
		print('```')
	print('{} tests done, {} passed, {} failed, timeout {}'.format(len(tests_list), passed, failed, timeout))
	print('-'*100)
	expected_to_fail_tests = (set(failed_tests) & set(targets_should_be_failed))
	unexpected_to_fail_tests = (set(failed_tests) - set(targets_should_be_failed))
	print('following tests are failed! ({}, expected)'.format(len(expected_to_fail_tests)))
	for i in expected_to_fail_tests:
		print('\t{}'.format(i))
	print('-'*100)
	print('following tests are failed! ({}, unexpected)'.format(len(unexpected_to_fail_tests)))
	for i in unexpected_to_fail_tests: 
		print('\t{}'.format(i))
	print('-'*100)
	subprocess.run(['stty', 'echo'])
	return exit_code
if __name__ == '__main__':
	sys.exit(main())
