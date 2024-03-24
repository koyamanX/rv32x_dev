set -ex

IMAGE_FILE="card.img"

if [ -d /root/scripts/tmp ]; then
    rm -rf /root/scripts/tmp
fi

#build linux kernel(blkdev is mmc)

pushd /root/software/linux
make ARCH=riscv CROSS_COMPILE=riscv32-unknown-linux-gnu- rv32xSoC_defconfig
make CFLAGS="-march=rv32ima_zicsr -mabi=ilp32" LDFLAGS="-march=rv32ima_zicsr -mabi=ilp32" ARCH=riscv CROSS_COMPILE=riscv32-unknown-linux-gnu- all -j`nproc`
popd

#build payload

pushd /root/software/opensbi
make clean
make -C ../bootrom -B #build device tree
make CROSS_COMPILE=riscv32-unknown-elf- PLATFORM_DIR=platform PLATFORM=rv32xsoc FW_PAYLOAD_PATH=/root/software/linux/arch/riscv/boot/Image FW_FDT_PATH=/root/software/bootrom/rv32xsoc.dtb
popd

#build rootfs
pushd /root/software/buildroot
make rv32xsoc_defconfig
make
popd

mkdir -p /root/scripts/output
pushd /root/scripts/output
#create bootable image
dd if=/dev/zero of=${IMAGE_FILE} count=1 bs=500M
#part 1: 40MiB bootable
#part 2: bs-40MiB
echo -e \
"n\n\
p\n\
1\n\
\n\
80K\n\
a\n\
n\n\
p\n\
2\n\
81921\n\
\n\
p\n\
w\n" | fdisk ${IMAGE_FILE}

LOOPDEV=$(losetup --find --show --partscan ${IMAGE_FILE})

# drop the first line, as this is our LOOPDEV itself, but we only want the child partitions
PARTITIONS=$(lsblk --raw --output "MAJ:MIN" --noheadings ${LOOPDEV} | tail -n +2)
COUNTER=1
for i in $PARTITIONS; do
    MAJ=$(echo $i | cut -d: -f1)
    MIN=$(echo $i | cut -d: -f2)
    if [ ! -e "${LOOPDEV}p${COUNTER}" ]; then mknod ${LOOPDEV}p${COUNTER} b $MAJ $MIN; fi
    COUNTER=$((COUNTER + 1))
done
# https://github.com/moby/moby/issues/27886#issuecomment-417074845

dd if=/root/software/opensbi/build/platform/rv32xsoc/firmware/fw_payload.bin of=${LOOPDEV}p1
dd if=/root/software/buildroot/output/images/rootfs.ext2 of=${LOOPDEV}p2

losetup -d ${LOOPDEV}
popd