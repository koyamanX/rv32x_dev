FROM ubuntu:latest
MAINTAINER ckoyama<ckoyama.1996@gmail.com>

# PATH to quartus, nslcore in your local system
# Quartus is not required if you don't use FPGA
ENV QUARTUS=/opt/intelFPGA_lite/20.1/quartus/bin
ENV NSLCORE=/opt/nslcore/linux/x86_64
# Install directory for RISCV toolchain
ENV RISCV=/opt/riscv
ENV PATH=$RISCV/bin:$QUARTUS:$NSLCORE:$PATH
ENV USER=root
ENV HOME=/root
ENV DEBIAN_FRONTEND=noninteractive

WORKDIR ${HOME}
# Install deps
RUN apt update && apt install -y iverilog libsystemc-dev libsystemc libsystemc-doc verilator gtkwave build-essential autoconf automake autotools-dev curl python3 libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev git curl wget libiberty-dev
# Install gcc and binutils
RUN git clone https://github.com/riscv/riscv-gnu-toolchain
WORKDIR ${HOME}/riscv-gnu-toolchain
# Do not clone qemu (takes long time)
RUN git -c submodule."qemu".update=none submodule update --init --recursive --jobs=10 --progress
RUN mkdir ${HOME}/riscv-gnu-toolchain/build
WORKDIR ${HOME}/riscv-gnu-toolchain/build
RUN ../configure --prefix=/opt/riscv --with-arch=rv32imac --with-abi=ilp32
RUN make linux -j $(nproc)
RUN make newlib -j $(nproc)
# Install BFD library
RUN mkdir ${HOME}/riscv-gnu-toolchain/riscv-binutils/build
WORKDIR ${HOME}/riscv-gnu-toolchain/riscv-binutils/build
RUN ../configure --prefix=/opt/riscv --target=riscv32-elflittle --enable-commonbfdlib --disable-gdb --enable-install-libbfd 
RUN make -j $(nproc)
RUN make install-bfd -j $(nproc)
RUN make install-opcodes -j $(nproc)
WORKDIR ${HOME}
# Remove build directory
RUN rm -rf ${HOME}/riscv-gnu-toolchain/build ${HOME}/riscv-gnu-toolchain/riscv-binutils/build 
# RUN rm -rf ${HOME}/riscv-gnu-toolchain
WORKDIR ${HOME}
RUN git clone --recursive https://github.com/riscv/riscv-isa-sim.git
RUN mkdir riscv-isa-sim/build
WORKDIR ${HOME}/riscv-isa-sim/build
RUN ../configure --prefix=/opt/riscv --with-isa=RV32IMAC --with-priv=M --enable-commitlog=yes
RUN make -j $(nproc)
RUN make install -j $(nproc)
WORKDIR ${HOME}

WORKDIR ${HOME}
