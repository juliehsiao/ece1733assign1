

CC=gcc
CFLAGS=-I.
OBJ=common_types.o cubical_function_representation.o blif_parser.o blif_parser.tab.o blif_reader.o assign1.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

assign1: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o

