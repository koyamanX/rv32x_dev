#!/usr/bin/env python3

import sys

def main(argc, argv):
	lines = [line.rstrip() for line in sys.stdin.readlines()]
	while '' in lines:
		lines.remove('')
	while '\n' in lines:
		lines.remove('\n')
	for line in lines:
		if(line[0] == '#'):
			continue
		print(line)
	return 0	

if __name__ == '__main__':
	sys.exit(main(len(sys.argv), sys.argv))
