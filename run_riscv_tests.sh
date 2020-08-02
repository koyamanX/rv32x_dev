#!/bin/bash

./run.sh 'make -C software/riscv-tests/isa/ XLEN=32'
if [[ -z "$SIM_PATH" ]]; then
	SIM_PATH=simulation
fi

./run.sh 'make clean -C '$SIM_PATH
./run.sh 'make run_riscv_tests -C '$SIM_PATH
