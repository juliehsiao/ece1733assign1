GCC = gcc
# need to add -m32 flag if you are using a 64-bit system due to pointer/int conversions
CFLAGS = -c -g -Wall -l. -lfl -m32 #-O3
LDFLAGS = -lm -m32 

SOURCES = 	assign1.c \
		  	common_types.c \
			blif_parser.tab.c \
			blif_parser.c \
		  	blif_reader.c \
		  	cubical_function_representation.c \
		  	findMinCover.c \
		  	findPI.c

OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = assign1

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(GCC) $(OBJECTS) $(LDFLAGS) -o $@

assign1.o: assign1.c
	$(GCC) $(CFLAGS) assign1.c

blif_parser.o: blif_parser.c
	$(GCC) $(CFLAGS) blif_parser.c

blif_parser.tab.o: blif_parser.tab.c
	$(GCC) $(CFLAGS) blif_parser.tab.c

blif_reader.o: blif_reader.c
	$(GCC) $(CFLAGS) blif_reader.c

common_types.o: common_types.c
	$(GCC) $(CFLAGS) common_types.c

cubical_function_representation.o: cubical_function_representation.c
	$(GCC) $(CFLAGS) cubical_function_representation.c

findPI.o: findPI.c
	$(GCC) $(CFLAGS) findPI.c

findMinCover.o: findMinCover.c
	$(GCC) $(CFLAGS) findMinCover.c

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
