#!/bin/bash
set -ex

git clone --depth=1 --recurse-submodules --shallow-submodules -j4  https://github.com/koyamanX/riscv-openocd
pushd riscv-openocd &> /dev/null
	./bootstrap
	mkdir build
	pushd build &> /dev/null
		../configure --prefix=/opt/riscv-openocd
		make -j $(nproc) && make install
	popd &> /dev/null
popd &> /dev/null
rm -rf riscv-openocd

