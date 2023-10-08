CC = gcc
LINT.c = splint

CFLAGS = -O0 -g3 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion 

OBJECTS = qfile.o qattr.o test.o qdefs.o
SOURCES = $(OBJECTS:.o=.c)

all:
	echo $(LINT.c)

clean:
	rm *.o

.PHONY: docs
docs:
	doxygen Doxyfile

lint:
	$(LINT.c) $(SOURCES)

test: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

test2: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJECTS): %.o: %.c
