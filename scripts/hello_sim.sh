TARGET=helloworld
pushd /root/software/helloworld
make TEST=$TARGET
./rv32x_simulation $TARGET.elf
popd