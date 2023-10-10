IMAGE_FILE="card.img"

dd if=/dev/zero of=${IMAGE_FILE} count=1 bs=120M
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
dd if=/root/software/buildroot/output/images/rootfs.ext2 of=block_device.img of=${LOOPDEV}p2

losetup -d ${LOOPDEV}


