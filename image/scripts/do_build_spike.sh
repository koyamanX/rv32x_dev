#!/bin/bash
set -ex

export RISCV=/opt/riscv
git clone --depth=1 https://github.com/riscv/riscv-isa-sim.git
pushd riscv-isa-sim &> /dev/null
	./configure --prefix=/opt/riscv --enable-commitlog
	make -j $(nproc)
	make install
popd &> /dev/null
rm -rf riscv-isa-sim
