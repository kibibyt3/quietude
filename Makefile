CC = gcc
DEBUG_CC = gcc

CFLAGS = -O0 -g3
DEBUG_CFLAGS = -O0 -g3

debug_objects = qattr.o test.o qdefs.o
objects = qattr.o

test: $(debug_objects)
	$(DEBUG_CC) $(DEBUG_CFLAGS) -o $@ $^

$(objects): %.o: %.c

clean:
	rm *.o

.PHONY: docs
docs:
	doxygen Doxyfile
