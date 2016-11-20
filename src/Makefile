ir_receive : ir_receive.o
	cc -o ir_receive ir_receive.o
ir_transmit : ir_transmit.o
	cc -o ir_transmit ir_transmit.o
ir_receive.o : ir_receive.c ir_shared.h
	cc -c ir_receive.c
ir_transmit.o : ir_transmit.c ir_shared.h
	cc -c ir_transmit.c
clean :
	rm ir_receive ir_transmit ir_receive.o ir_transmit.o