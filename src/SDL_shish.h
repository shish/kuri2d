#ifndef _SDL_SHISH_H_
#define _SDL_SHISH_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
/* #include "SDL_mixer.h" */

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================= */

extern SDL_Window* window;
extern SDL_Renderer* renderer;

int SDL_ShishInit();

/* ======================================================= */

typedef struct BasicSound {
    SDL_AudioSpec *sound;
    Uint32 length;
    Uint8 *data;
} BasicSound;

/* ======================================================= */

void toggleFullscreen(SDL_Window* window);

/* ======================================================= */

void fillRect(int x, int y, int w, int h, int c);

void drawString(TTF_Font *font, char *text, SDL_Color color, int x, int y);
SDL_Texture *loadTexture(const char *name);
void drawTexture(SDL_Texture *img, int x, int y);

void screenshot(char *name);

/* ======================================================= */

void playSound(int chan, BasicSound *snd);

/* ======================================================= */

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* !_SHISH_SDL_H_ */

