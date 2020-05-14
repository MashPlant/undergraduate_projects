all:
	ghdl -a --ieee=synopsys *.vhd
	ghdl -e --ieee=synopsys tb
	ghdl -r tb  --vcd=led.vcd
	# gtkwave led.vcd

clean:
	rm *.o *.vcd tb *.cf
