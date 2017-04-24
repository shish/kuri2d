VERSION = 0.9.1
CFLAGS = \
	-I/opt/local/include/ \
	-DVERSION=\"$(VERSION)\" \
	-pedantic -Wall -std=c99 -O2
LDFLAGS = \
	-L/opt/local/lib/ \
	-lSDL2main -lSDL2_ttf -lSDL2_image -lSDL2
	# -mwindows

all: kuri2d kuri2d.html

kuri2d: src/*.c Makefile
	cc   src/*.c $(CFLAGS) -o kuri2d      $(LDFLAGS)

kuri2d.html: src/*.c Makefile
	emcc src/*.c $(CFLAGS) -o kuri2d.html $(LDFLAGS) \
	-s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_TTF=2 \
	--preload-file data

dist: kuri2d
	tar -cvzf kuri2d-$(VERSION)-src.tar.gz src/*.c src/*.h Makefile $(GNUFILES)
	tar -czvf kuri2d-$(VERSION)-x86_64.tar.gz $(BINARIES) $(GNUFILES) data

clean:
	rm -f kuri2d*
