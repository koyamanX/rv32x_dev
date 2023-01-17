#!/usr/bin/env python3

import glob
import os
import sys
import subprocess
import shutil

NO_LOG=""

simulator='./rv32x_simulation'
riscv_tests_directory = '../software/riscv-tests/isa/'
simulation_log_directory = './logs/'
targets = [
	'rv32ui-p-*',
	'rv32ui-v-*',
	'rv32um-p-*',
	'rv32um-v-*',
	'rv32mi-p-*',
	'rv32ua-p-*',
	'rv32ua-v-*',
	'rv32si-p-*',
]
targets_should_be_failed = [
	'rv32si-p-dirty',
]
targets_to_exclude = [
	'rv32mi-p-breakpoint',
]
failed=0
failed_tests = []
timeout=0
passed=0
skipped=0
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
	global passed, failed, timeout, skipped
	for t, a in zip(tests_list, tests_assertion):
		if(t in targets_to_exclude):
			print('{:20}\tskipped'.format(t))
			skipped += 1
			continue
		try:
			if NO_LOG == "no_log":
				subprocess.run([simulator, riscv_tests_directory+t], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True, timeout=10)
			else:
				subprocess.run([simulator, riscv_tests_directory+t, '--print-all'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True, timeout=10)
				
		except subprocess.CalledProcessError as err:
			if(a == 'should_be_passed'):
				print(err.returncode)
				print('{:20}\t{:20}  and it\'s FAILED\tNG'.format(t, a))
				failed_tests.append(t)
				failed+=1
			else:
				print('{:20}\t{:20}  and it\'s FAILED\t'.format(t, a))
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
			if NO_LOG == "no_log":
				nop=1
			else:
				shutil.move(t+'.log', simulation_log_directory)
		if(a == 'should_be_passed'):
			print('{:20}\t{:20}  and it\'s PASSED\tOK'.format(t, a))
			passed+=1
		elif(a == 'should_be_failed'):
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
	exit_code = 0 if(len(targets_should_be_failed) == failed) else failed

	if md_opt == 1:
		print('```')
		return exit_code
	print('{} tests done, {} passed, {} failed, timeout {}, skipped {}'.format(len(tests_list), passed, failed, timeout, skipped))
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
	NO_LOG = str(sys.argv[1])
	sys.exit(main())
