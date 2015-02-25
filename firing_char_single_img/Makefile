CXX=clang++
CXX_ARGS=-stdlib=libc++ -std=c++1y -Wall -pedantic -Wextra -Warray-bounds \
	-Wunreachable-code -pipe $(shell sdl2-config --cflags)
CXX_DEBUG=-O3 -march=native -DNDEBUG
CXX_LIBS=$(shell sdl2-config --libs) -lSDL2_image

CXX_BASE_CMD=$(CXX) $(CXX_ARGS) $(CXX_DEBUG)

.cpp.o:
	$(CXX_BASE_CMD) -c $<

$(shell $(CXX_BASE_CMD) -MM *.cpp > deps)

include deps

OBJS=EngCharacter.o Graphical.o xSDL.o xSDL_image.o main.o \
  Atlas.o ParticlesSystem.o

build: $(OBJS)
	$(CXX_BASE_CMD) *.o -o prog $(CXX_LIBS)
	rm -f deps

clean:
	rm -rf *.o prog

run: build
	./prog
