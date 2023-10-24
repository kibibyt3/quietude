CC = gcc
LINT.c = splint

CFLAGS = -I include -Og -g3 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion
LDLIBS = -lncurses
LINTFLAGS = -I include -checks +posixlib

GAME_OBJECTS = ./src/qfile.o ./src/qattr.o ./src/qdefs.o ./src/qerror.o ./src/qwalkw.o ./src/qwalkl.o ./src/qwalkio.o
GAME_SOURCES = $(GAME_OBJECTS:.o=.c)

TEST_OBJECTS = ./src/test.o
TEST_SOURCES = $(TEST_OBJECTS:.o=.c)

DEVEL_OBJECTS = ./src/devel_walk.o
DEVEL_SOURCES = $(DEVEL_OBJECTS:.o=.c)

all: test devel_walk

test: $(GAME_OBJECTS) $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

devel_walk: $(GAME_OBJECTS) $(TEST_OBJECTS) $(DEVEL_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

clean:
	$(RM) src/*.o test devel_walk

.PHONY: docs
docs:
	doxygen Doxyfile

lint:
	$(LINT.c) $(LINTFLAGS) $(GAME_SOURCES) $(TEST_SOURCES) $(DEVEL_SOURCES)

$(GAME_OBJECTS): %.o: %.c

$(TEST_OBJECTS): %.o: %.c

$(DEVEL_OBJECTS): %.o: %.c
