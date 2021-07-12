FROM ubuntu:20.04
MAINTAINER ckoyama<ckoyama.1996@gmail.com>

ENV NSLCORE=/opt/nslcore/linux/x86_64

# Quartus is not required if you don't use FPGA
ENV QUARTUS=/opt/intelFPGA_lite/20.1/quartus/bin

# Install directory for RISCV toolchain
ENV RISCV=/opt/riscv
ENV PATH=$RISCV/bin:$QUARTUS:$NSLCORE:$PATH
ENV USER=user
ENV HOME=/home/user
ENV DEBIAN_FRONTEND=noninteractive

WORKDIR ${HOME}
# Install deps
RUN apt update && apt install -y iverilog libsystemc-dev libsystemc libsystemc-doc verilator gtkwave build-essential autoconf automake autotools-dev curl python3 libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev git curl wget libiberty-dev

RUN mkdir -p /opt/nslcore
RUN curl https://ip-arch.jp/unsupported/nslcore-x86_64-linux-20210228-90.tar.gz | tar xvz -C /opt/nslcore

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

WORKDIR ${NSLCORE}
COPY ./nsl2vl.patch ./
RUN patch < ./nsl2vl.patch

RUN useradd -m user
WORKDIR ${HOME}
USER ${USER}
