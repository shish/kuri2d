VERSION = 0.9.0
CC = gcc
CFLAGS = -ggdb \
	-I/usr/include/SDL -IC:/mingw/include/SDL \
	-DVERSION=\"$(VERSION)\" \
	-DENABLE_BINRELOC=1 \
	-pedantic -Wall -std=c99
	# -ansi -std=c99 \
	# -03
LDFLAGS = -lSDLmain -lSDL_ttf -lSDL_image -lpng -lSDL -mwindows

OBJECTS = kuri2d.o main.o editor.o menu.o logic.o init.o exit.o SDL_shish.o binreloc.o
GNUFILES = doc/* kuri2d.lsm
BINARIES = kuri2d # kuri2d.exe 

.SUFFIXES: .c

%.o: %.c kuri2d.h
	$(CC) $(CFLAGS) -o $@	-c $<

all: kuri2d 

check: *.c
	splint $<

kuri2d: $(OBJECTS)
	$(CC) $(OBJECTS) -o kuri2d $(LDFLAGS)

dist: kuri2d
	tar -cvzf kuri2d-$(VERSION)-src.tar.gz *.c *.h Makefile $(GNUFILES)
	tar -czvf kuri2d-$(VERSION)-i386.tar.gz $(BINARIES) $(GNUFILES)
	tar -czvf kuri2d-$(VERSION)-data.tar.gz data

clean:
	rm -f kuri2d $(OBJECTS)

install:
	cp kuri2d /usr/local/games/bin/
	mkdir /usr/local/share/games/kuri2d
	cp -r data /usr/local/share/games/kuri2d/

uninstall:
	rm -f /usr/local/games/kuri2d
	rm -rf /usr/local/share/games/kuri2d/

