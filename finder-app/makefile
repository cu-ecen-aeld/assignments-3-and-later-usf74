
var=gcc
ifdef CROSS_COMPILE
	ifeq (${CROSS_COMPILE},aarch64-none-linux-gnu-)
		var=${CROSS_COMPILE}gcc
	endif
endif
all: writer.c
	${var} -o writer writer.c
clean:
	rm -f writer
	rm -f *.o
