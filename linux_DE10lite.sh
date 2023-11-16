QUARTUS_DIR=$1
MMC_DEV=$2

if [ -n "$QUARTUS_DIR" ]; then
    pushd $QUARTUS_DIR
    QUARTUS_DIR=$(pwd)
    popd
fi

./run.sh "bash -ic scripts/make_bootable_img.sh"
./run.sh "bash -ic scripts/make_bootstrap_mif.sh"

make -C ./FPGA -B QUARTUS_DIR=$QUARTUS_DIR

if [ -n "$MMC_DEV" ]; then
    dd if=scripts/output/card.img of=$MMC_DEV
fi

#make -C ./FPGA download
