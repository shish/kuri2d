#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <SDL/SDL.h>
#include <sys/stat.h>
#include "SDL_shish.h"

static SDL_Surface *rootSurface;
static SDL_Surface *currentSurface;
Uint32 currentColor = 0xFFFF7700;

void refresh() {
    (void)SDL_Flip(rootSurface);
}

void screenshot(char *name) {
	int n=0;
	char filename[32];
	struct stat buffer;
	for(n=0; n<9999; n++) {
		sprintf(filename, "%s_%04i.bmp", name, n);
		if(stat(filename, &buffer) == -1) break;
	}
	SDL_SaveBMP(SDL_GetVideoSurface(), filename);
}

void setCurrentSurface(SDL_Surface *surf) {
    if(surf == NULL) {
        printf("Warning: surface is null\n");
        return;
    }
    currentSurface = surf;
}

SDL_Surface *getCurrentSurface() {
    return currentSurface;
}

void setRootSurface(SDL_Surface *surf) {
    if(surf == NULL) {
        printf("Warning: trying to set rootSurf to null!\n");
        return;
    }
    rootSurface = surf;
}

SDL_Surface *getRootSurface() {
    return rootSurface;
}

void fillRect(int x, int y, int w, int h, int c) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    (void)SDL_FillRect(currentSurface, &rect, c);
}

void lockScreen() {
	if(SDL_MUSTLOCK(rootSurface)) {
		if(SDL_LockSurface(rootSurface) < 0) return;
	}
}

void unlockScreen() {
	if(SDL_MUSTLOCK(rootSurface)) {
		SDL_UnlockSurface(rootSurface);
	}
}

void drawImage(SDL_Surface *img, int x, int y) {
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	(void)SDL_BlitSurface(img, NULL, currentSurface, &dest);
}

void drawImageClipped(SDL_Surface *img, int x, int y,
		int cx, int cy, int cw, int ch) {
	SDL_Rect clip, dest;
	clip.x = cx;
	clip.y = cy;
	clip.w = cw;
	clip.h = ch;
	dest.x = x;
	dest.y = y;
	(void)SDL_BlitSurface(img, &clip, currentSurface, &dest);
}

void drawImageAlpha(SDL_Surface *img, int x, int y, Uint8 a) {
	SDL_Rect dest;
	if(img == NULL) return;
	img->format->alpha = a;
	dest.x = x;
	dest.y = y;
	(void)SDL_BlitSurface(img, NULL, currentSurface, &dest);
	img->format->alpha = 0xFF;
}

void playSound(/*@ unused @*/ int channel, /*@ unused @*/ BasicSound* data) {
	/* FIXME: implement this */
}
