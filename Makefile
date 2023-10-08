CC = gcc
LINT.c = splint

CFLAGS = -O0 -g3 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion 

OBJECTS = ./src/qfile.o ./src/qattr.o ./src/test.o ./src/qdefs.o ./src/mode.o
SOURCES = $(OBJECTS:.o=.c)

all: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) src/*.o test

.PHONY: docs
docs:
	doxygen Doxyfile

lint:
	$(LINT.c) -checks $(SOURCES)

$(OBJECTS): %.o: %.c
