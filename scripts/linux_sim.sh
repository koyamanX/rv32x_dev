set -ex

if [ -d /root/scripts/tmp ]; then
    rm -rf /root/scripts/tmp
fi
#build initramfs

pushd /root/software/busybox
make ARCH=riscv CROSS_COMPILE=riscv32-unknown-linux-gnu- defconfig
make install ARCH=riscv CROSS_COMPILE=riscv32-unknown-linux-gnu- CFLAGS=-static -j `nproc`
popd

mkdir -p /root/scripts/tmp/fsbuild
pushd /root/scripts/tmp/fsbuild
cp -r /root/software/busybox/_install/*/ ./
mkdir -p dev etc/init.d lib/modules mnt proc tmp var/log var/run var/tmp
pushd dev
mknod console c 5 1
mknod hvc c 229 0
mknod loop0 b 7 0
mknod null c 1 3
ln -s null tty2
ln -s null tty3
ln -s null tty4
popd
pushd etc/init.d
touch rcS
chmod 755 rcS
popd
ln -s bin/busybox init
popd

#build linux kernel(blkdev is ram)

pushd /root/software/linux
sed -i -e 's/CONFIG_INITRAMFS_SOURCE=.*/CONFIG_INITRAMFS_SOURCE=\"\/root\/scripts\/tmp\/fsbuild\"/g' /root/software/linux/arch/riscv/configs/rv32xSoC_ramfs_defconfig
make ARCH=riscv CROSS_COMPILE=riscv32-unknown-linux-gnu- rv32xSoC_ramfs_defconfig
make CFLAGS="-march=rv32ima_zicsr -mabi=ilp32" LDFLAGS="-march=rv32ima_zicsr -mabi=ilp32" ARCH=riscv CROSS_COMPILE=riscv32-unknown-linux-gnu- all -j`nproc`
popd

#build payload

pushd /root/software/opensbi
make clean
make -C ../bootrom -B #build device tree
make CROSS_COMPILE=riscv32-unknown-elf- PLATFORM_DIR=platform PLATFORM=rv32xsoc FW_PAYLOAD_PATH=/root/software/linux/arch/riscv/boot/Image FW_FDT_PATH=/root/software/bootrom/rv32xsoc.dtb
popd

#build simulator
make -C /root/simulation/bootrom -B LOADEROPT=-DBINSIZE=0\ -DHEAD=0x80000000
make -C /root/simulation -B SIMOPT=-DKERNEL_START_ADDR=0x80400000

pushd /root/scripts/tmp
cp /root/simulation/bootrom.hex ./
cp /root/simulation/rv32x_simulation ./
ln -s /root/software/opensbi/build/platform/rv32xsoc/firmware/fw_payload.elf linux
clear 
./rv32x_simulation linux
popd

rm -rf /root/scripts/tmp