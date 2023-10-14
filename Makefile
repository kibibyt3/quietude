CC = gcc
LINT.c = splint

CFLAGS = -I include -O0 -g3 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion
LINTFLAGS = -I include -checks

OBJECTS = ./src/qfile.o ./src/qattr.o ./src/test.o ./src/qdefs.o ./src/qerror.o ./src/qwalkw.o ./src/qwalkl.c
SOURCES = $(OBJECTS:.o=.c)

all: test

test: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) src/*.o test

.PHONY: docs
docs:
	doxygen Doxyfile

lint:
	$(LINT.c) $(LINTFLAGS) $(SOURCES)

$(OBJECTS): %.o: %.c
