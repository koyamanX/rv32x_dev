#./gtk.sh XXXXXXX.vcd

if [ -n "$1" ]; then
    sed -i -e "5s/.*/[dumpfile] \"$1\"/" rv32x.gtkw
fi
gtkwave rv32x.gtkw