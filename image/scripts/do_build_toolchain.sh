#!/bin/bash
set -ex
CTNG_VERSION=1.26.0
BINUTILS_VERSION=2.40

mkdir build
pushd build &> /dev/null
	wget http://crosstool-ng.org/download/crosstool-ng/crosstool-ng-$CTNG_VERSION.tar.bz2
	tar -xvf crosstool-ng-$CTNG_VERSION.tar.bz2
	wget https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.xz
	tar -xvf binutils-$BINUTILS_VERSION.tar.xz
	git clone https://github.com/twinkletia/linux --depth=1 -b rv32xSoC

	mkdir binutils-$BINUTILS_VERSION/build
	pushd binutils-$BINUTILS_VERSION/build &> /dev/null
		../configure --prefix=/opt/riscv --target=riscv32-elflittle --enable-commonbfdlib --disable-gdb --enable-install-libbfd --enable-install-libiberty
		make -j $(nproc)
		make install-bfd -j $(nproc)
		make install-opcodes -j $(nproc)
		make install-libsframe -j $(nproc)
		make install-libiberty -j $(nproc)
	popd &> /dev/null

	pushd crosstool-ng-$CTNG_VERSION &> /dev/null
		./bootstrap
		mkdir build
		pushd build &> /dev/null
		../configure
		make
		make install
		popd &> /dev/null
	popd &> /dev/null

	ct-ng defconfig DEFCONFIG=../riscv_cross_newlib_defconfig
	ct-ng build
	ct-ng defconfig DEFCONFIG=../riscv_cross_linux_defconfig
	ct-ng build
	export PATH=/opt/riscv/bin:$PATH
	ct-ng defconfig DEFCONFIG=../riscv_native_linux_defconfig
	ct-ng build
popd &> /dev/null
rm -rf build
