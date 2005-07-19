#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <SDL/SDL.h>
#include <sys/stat.h>
#include "SDL_shish.h"

SDL_Surface *rootSurface;
SDL_Surface *currentSurface;
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

void putPixel(int x, int y, Uint8 r, Uint8 g, Uint8 b) {
	Uint32 color = SDL_MapRGB(currentSurface->format, r, g, b);

	switch(currentSurface->format->BytesPerPixel) {
		case 1:
			{
				Uint8 *bufp;
				bufp = (Uint8 *)currentSurface->pixels +
					y*currentSurface->pitch + x;
				*bufp = color;
			}
			break;

		case 2:
			{
				Uint16 *bufp;
				bufp = (Uint16 *)currentSurface->pixels +
					y*currentSurface->pitch/2 + x;
				*bufp = color;
			}
			break;

		case 3:
			{
				Uint8 *bufp;
				bufp = (Uint8 *)currentSurface->pixels +
					y*currentSurface->pitch + x*3;
				if(SDL_BYTEORDER == SDL_LIL_ENDIAN) {
					bufp[0] = color;
					bufp[1] = color >> 8;
					bufp[2] = color >> 16;
				}
				else {
					bufp[2] = color;
					bufp[1] = color >> 8;
					bufp[0] = color >> 16;
				}
			}
			break;

		case 4:
			{
				Uint32 *bufp;
				bufp = (Uint32 *)currentSurface->pixels +
					y*currentSurface->pitch/4 + x;
			}
			break;
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

void playSound(int channel, BasicSound* data) {
	/* FIXME: implement this */
}
