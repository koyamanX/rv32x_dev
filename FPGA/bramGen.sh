FILENAME="ram_1p"

#ip-generate ram_xport only supports Arria 10 and Strarix 10

ip-generate --file-set=quartus_synth \
--component-name=ram_1port \
--output-name=$FILENAME \
--system-info=DEVICE_FAMILY=\"Cyclone V\" \
--component-param=GUI_MIFfilename=\"program.mif\" \
--component-param=GUI_BlankMemory=\"1\" \
--component-param=GUI_RegOutput=\"false\" \
--component-param=GUI_NUMWORDS_A=\"16384\" \
#--component-param=GUI_WIDTH_A=\"32\" \

#for cyclone v

sed -i -e "s/\.address2_.*/\x08/g" $FILENAME/$FILENAME.v
sed -i -e "s/\.eccenc.*/\x08/g" $FILENAME/$FILENAME.v
sed -i -e "s/\.sclr.*/\x08/g" $FILENAME/$FILENAME.v
sed -i -e "s/altsyncram_component.outdata_sclr_a.*/\x08/g" $FILENAME/$FILENAME.v
sed -i -e "s/altsyncram_component.enable_force_to_zero.*/\x08/g" $FILENAME/$FILENAME.v
sed -z -i -e "s/\x08\n[\ ]*//g" $FILENAME/$FILENAME.v

FILENAME="rom_1p"

#ip-generate ram_xport only supports Arria 10 and Strarix 10

ip-generate --file-set=quartus_synth \
--component-name=rom_1port \
--output-name=$FILENAME \
--system-info=DEVICE_FAMILY=\"Cyclone V\" \
--component-param=GUI_MIFfilename=\"bootrom.mif\" \
--component-param=GUI_BlankMemory=\"1\" \
--component-param=GUI_RegOutput=\"false\" \
--component-param=GUI_NUMWORDS_A=\"1024\" \
--component-param=GUI_WIDTH_A=\"32\" \

#for cyclone v

sed -i -e "s/\.address2_.*/\x08/g" $FILENAME/$FILENAME.v
sed -i -e "s/\.eccenc.*/\x08/g" $FILENAME/$FILENAME.v
sed -i -e "s/\.sclr.*/\x08/g" $FILENAME/$FILENAME.v
sed -i -e "s/altsyncram_component.outdata_sclr_a.*/\x08/g" $FILENAME/$FILENAME.v
sed -i -e "s/altsyncram_component.enable_force_to_zero.*/\x08/g" $FILENAME/$FILENAME.v
sed -z -i -e "s/\x08\n[\ ]*//g" $FILENAME/$FILENAME.v