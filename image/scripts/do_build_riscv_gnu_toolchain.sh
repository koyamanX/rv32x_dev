#!/bin/bash
set -ex

git -c submodule."qemu".update=none clone -b 2021.08.11 --depth=1 --recurse-submodules -j4 https://github.com/riscv/riscv-gnu-toolchain
pushd riscv-gnu-toolchain &> /dev/null
	mkdir build
	pushd build &> /dev/null
		../configure --prefix=/opt/riscv --with-arch=rv32ima --with-abi=ilp32
		make linux -j $(nproc)
		make newlib -j $(nproc)
	popd &> /dev/null
	mkdir riscv-binutils/build
	pushd riscv-binutils/build &> /dev/null
		../configure --prefix=/opt/riscv --target=riscv32-elflittle --enable-commonbfdlib --disable-gdb --enable-install-libbfd 
		make -j $(nproc)
		make install-bfd -j $(nproc)
		make install-opcodes -j $(nproc)
	popd &> /dev/null
popd &> /dev/null
rm -rf riscv-gnu-toolchain

