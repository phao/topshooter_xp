CC=clang
CC_ARGS=-std=c99 -Wall -pedantic -Wextra -Warray-bounds -Wunreachable-code \
	-pipe $(shell sdl2-config --cflags)
CC_DEBUG=-g3
CC_LIBS=$(shell sdl2-config --libs) -lSDL2_image -lm

CC_BASE_CMD=$(CC) $(CC_ARGS) $(CC_DEBUG)

.c.o:
	$(CC_BASE_CMD) -c $<

$(shell $(CC_BASE_CMD) -MM *.c > deps)

include deps

OBJS=Engineer.o Graphical.o main.o Atlas.o Particles.o

build: $(OBJS)
	$(CC_BASE_CMD) *.o -o prog $(CC_LIBS)
	rm -f deps

clean:
	rm -rf *.o prog

run: build
	./prog
