#!/bin/bash
target="	\
	../software/riscv-tests/isa/rv32ui-p-* \
	../software/riscv-tests/isa/rv32um-p-* \
	"
	#../software/riscv-tests/isa/rv32mi-p-* \
target=$(echo $target | sed -e 's/\.dump//g' | xargs -n1 | sort -u | xargs | sed 's# # #g')
sim=rv32x_simulation
pass=0
fail=0
total=0
for i in $target; do
	let total=$total+1
	if [ ! -e logs ]; then
		mkdir logs
	fi
	timeout 4 ./$sim $i 2> /dev/null
	#./$sim $i 2> /dev/null
	result="$?"
	mv "$(basename $i).log" logs
	if [ "$result" = "0" ] ; then
		let pass=$pass+1
		echo "$i is passed"
		continue
	elif [ "$result" = "124" ] ; then
		let fail=$fail+1
		echo "$i got infinite loop? (timeout)"
	else
		let fail=$fail+1
		echo "$i is failed"
	fi
	if [ ! -e failed ]; then
		mkdir failed
		cp rv32x.gtkw failed
	fi
	cp rv32x_simulation.vcd failed/$(basename $i).vcd
	cp $i failed/$(basename $i)
done
echo "Total: $total tests"
echo "$fail fails, $pass passes"
if [ $fail != 0 ]; then
	exit 1;
else
	exit 0;
fi
