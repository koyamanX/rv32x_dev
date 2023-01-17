#./gtk.sh XXXXXXX.vcd YYYYYYY.gtkw
formatFilename="rv32x.gtkw"
waveFilename="all.vcd"

if [ -n "$2" ]; then
    formatFilename="$2"
fi
if [ -n "$1" ]; then
    waveFilename="$1"
fi
sed -i -e "5s/.*/[dumpfile] \"$waveFilename\"/" $formatFilename
gtkwave $formatFilename