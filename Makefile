CC = gcc
DEBUG_CC = gcc

CFLAGS =
DEBUG_CFLAGS = -O0 -g3

debug_objects = qattr.o test.o
objects = qattr.o

test: $(debug_objects)
	$(DEBUG_CC) $(DEBUG_CFLAGS) -o $@ $^

$(objects): %.o: %.c

