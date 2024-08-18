
var=gcc
ifdef CROSS_COMPILE

	var=${CROSS_COMPILE}gcc

endif
all: writer.c
	${var} -o writer writer.c
clean:
	rm -f writer
	rm -f *.o
