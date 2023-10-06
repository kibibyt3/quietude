CC = gcc

CFLAGS = -O0 -g3 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion 

debug_objects = qfile.o qattr.o test.o qdefs.o
objects = qattr.o

test: $(debug_objects)
	$(CC) $(CFLAGS) -o $@ $^

$(objects): %.o: %.c

clean:
	rm *.o

.PHONY: docs
docs:
	doxygen Doxyfile
