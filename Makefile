CC = gcc
LINT.c = splint

CFLAGS = -I include -Og -g3 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion -pedantic
TEST_LDLIBS = -lncurses
DEVEL_LDLIBS = -lform -lncurses -lcdk
LINTFLAGS = -Iinclude -I/usr/local/include -checks +posixlib

GAME_OBJECTS = ./src/qfile.o ./src/qattr.o ./src/qdefs.o ./src/ioutils.o ./src/qerror.o ./src/qwalkw.o ./src/qwalkl.o ./src/qwalkio.o ./src/dialogue.o ./src/dialoguel.o
GAME_SOURCES = $(GAME_OBJECTS:.o=.c)

TEST_OBJECTS = ./src/test.o
TEST_SOURCES = $(TEST_OBJECTS:.o=.c)

DEVEL_OBJECTS = ./src/devel_walkl.o ./src/devel_walk_wins.o ./src/devel_walkio.o ./src/devel_walk.o
DEVEL_SOURCES = $(DEVEL_OBJECTS:.o=.c)

DEVEL_DIR = devel-utils

all: test devel_walk

test: $(GAME_OBJECTS) $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(TEST_LDLIBS)

devel_walk: $(GAME_OBJECTS) $(DEVEL_OBJECTS)
	$(CC) $(CFLAGS) -o $(DEVEL_DIR)/$@ $^ $(DEVEL_LDLIBS)

clean:
	$(RM) src/*.o test $(DEVEL_DIR)/devel_walk

.PHONY: docs
docs:
	doxygen Doxyfile

lint:
	echo \
		&& echo "---TEST LINT---" \
		&& $(LINT.c) $(LINTFLAGS) $(GAME_SOURCES) $(TEST_SOURCES) \
		&& echo \
		&& echo "---DEVEL LINT---" \
		&& $(LINT.c) $(LINTFLAGS) +partial $(GAME_SOURCES) $(DEVEL_SOURCES)

$(GAME_OBJECTS): %.o: %.c

$(TEST_OBJECTS): %.o: %.c

$(DEVEL_OBJECTS): %.o: %.c
