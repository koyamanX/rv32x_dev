
#pushd bram
echo "default setting\nWIDTH=8, NUMWORDS=8192, output q port=NO,\ninit_file=yes,empty(filled by makefile)"
qmegawiz -nonsilent wizard="RAM: 1-PORT" INTENDED_DEVICE_FAMILY="Cyclone V" maimmem.v
#popd