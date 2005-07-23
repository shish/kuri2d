#ifndef _SDL_SHISH_H_
#define _SDL_SHISH_H_

#include <SDL/SDL.h>
/* #include "SDL_mixer.h" */

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define SDL_SHISH_MAJOR		0
#define SDL_SHISH_MINOR		0
#define SDL_SHISH_PATCHLEVEL	0

#define C_RED		(0xFFFF0000)
#define C_GREEN		(0xFF00FF00)
#define C_BLUE		(0xFF0000FF)

/* ======================================================= */

typedef struct BasicSound {
    SDL_AudioSpec *sound;
    Uint32 length;
    Uint8 *data;
} BasicSound;

/* ======================================================= */

void setCurrentSurface(SDL_Surface *surf);
SDL_Surface *getCurrentSurface();

void setRootSurface(SDL_Surface *surf);
SDL_Surface *getRootSurface();

/* ======================================================= */

void fillRect(int x, int y, int w, int h, int c);
void drawBackground();
void refresh();

void lockScreen();
void unlockScreen();
void drawImage(SDL_Surface *img, int x, int y);
void drawImageAlpha(SDL_Surface *img, int x, int y, Uint8 a);
void drawImageClipped(SDL_Surface *img, int x, int y,
		      int cx, int cy, int cw, int ch);

void screenshot(char *name);

/* ======================================================= */

void playSound(int chan, BasicSound *snd);

/* ======================================================= */

#ifdef LIB
extern SDL_Font *currentFont;
extern SDL_Surface *rootSurface;
extern SDL_Surface *currentSurface;
extern Uint32 currentColor;
#endif

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* !_SHISH_SDL_H_ */

