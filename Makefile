CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99 -O2
LDFLAGS=-lm -lSDL2 -lSDL2_image -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer
SRC=$(wildcard src/*.c)
OBJS=$(SRC:src/%.c=obj/%.o)

.PHONY: all objs clean

all: objs space-war

objs:
	@mkdir -p obj

space-war: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf obj
	rm -f space-war
