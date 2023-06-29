FROM ubuntu:23.04

MAINTAINER ckoyama<ckoyama.1996@gmail.com>

ENV USER=root
ENV HOME=/root

RUN apt update && \
	DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
	gdb libc6-dbg strace libncurses-dev universal-ctags cpio unzip rsync\
	curl git sudo ca-certificates keyboard-configuration console-setup \
	build-essential cmake perl python3 make autoconf g++ flex bison ccache \
	libgoogle-perftools-dev numactl perl-doc libfl2 libfl-dev zlib1g \
	zlib1g-dev gtkwave libsystemc libsystemc-doc libsystemc-dev iverilog \
	vim bash automake autotools-dev libmpc-dev libmpfr-dev libgmp-dev gawk \
	texinfo gperf libtool patchutils bc libexpat-dev wget libiberty-dev \
	libftdi-dev pkg-config device-tree-compiler googletest help2man python3-pip libglib2.0-dev ninja-build && \
	rm -rf /var/lib/apt/lists/*

COPY scripts /root/scripts
WORKDIR /root/scripts

RUN ./do_build_riscv_gnu_toolchain.sh

RUN ./do_build_spike.sh

RUN ./do_build_verilator.sh

RUN ./do_install_nsl2vl.sh

WORKDIR /root
RUN cp scripts/env.sh /root/.env.sh && echo ". /root/.env.sh" >> .bashrc

USER ${USER}
WORKDIR ${HOME}
CMD ["/bin/bash"]
