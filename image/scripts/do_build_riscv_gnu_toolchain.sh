#!/bin/bash
set -ex

git clone -b 2023.06.09 https://github.com/riscv/riscv-gnu-toolchain

pushd riscv-gnu-toolchain &> /dev/null
	git submodule update --init --progress --depth=1 binutils dejagnu gcc gdb glibc newlib
	mkdir build
	pushd build &> /dev/null
		../configure --prefix=/opt/riscv --with-arch=rv32ima --with-abi=ilp32
		make linux -j $(nproc)
		make newlib -j $(nproc)
	popd &> /dev/null
	mkdir binutils/build
	pushd binutils/build &> /dev/null
		../configure --prefix=/opt/riscv --target=riscv32-elflittle --enable-commonbfdlib --disable-gdb --enable-install-libbfd 
		make -j $(nproc)
		make install-bfd -j $(nproc)
		make install-opcodes -j $(nproc)
	popd &> /dev/null
popd &> /dev/null
rm -rf riscv-gnu-toolchain

