CC = gcc
LINT.c = splint

CFLAGS = -O0 -g3 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion 

OBJECTS = qfile.o qattr.o test.o qdefs.o mode.o
SOURCES = $(OBJECTS:.o=.c)

all:
	echo $(LINT.c)

clean:
	$(RM) *.o test test2

.PHONY: docs
docs:
	doxygen Doxyfile

lint:
	$(LINT.c) -checks $(SOURCES)

test: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJECTS): %.o: %.c
