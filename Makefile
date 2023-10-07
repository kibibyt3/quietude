CC = gcc

CFLAGS = -O0 -g3 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion 

objects = qfile.o qattr.o test.o qdefs.o

test: $(objects)
	$(CC) $(CFLAGS) -o $@ $^

test2: $(objects)
	$(CC) $(CFLAGS) -o $@ $^

$(objects): %.o: %.c

clean:
	rm *.o

.PHONY: docs
docs:
	doxygen Doxyfile
