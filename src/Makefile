ir_receive : ir_receive.o
	gcc -o ir_receive ir_receive.o
ir_transmit : ir_transmit.o
	gcc -o ir_transmit ir_transmit.o
ir_receive.o : ir_receive.c ir_shared.h
	gcc -c ir_receive.c ir_shared.h -lmraa
ir_transmit.o : ir_transmit.c ir_shared.h
	gcc -c  ir_transmit.c ir_shared.h -lmraa
clean :
	rm ir_receive ir_transmit ir_receive.o ir_transmit.o