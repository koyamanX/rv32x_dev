#!/bin/bash
set -ex

git clone -b v4.210 --depth=1 https://github.com/verilator/verilator
pushd verilator &> /dev/null
	autoconf
	./configure --prefix=/opt/verilator
	make -j $(nproc)
	make install
popd &> /dev/null
rm -rf verilator
