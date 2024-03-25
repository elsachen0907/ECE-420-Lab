datatrim: datatrim.c
	gcc datatrim.c -o datatrim

main: main.c Lab4_IO.c Lab4_IO.h
	mpicc -g -Wall main.c Lab4_IO.c -o main -lm

clean:
	rm -f datatrim main data_input_link data_input_meta