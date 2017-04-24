#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_shish.h"

/* ======================================================= */

SDL_Window* window;
SDL_Renderer* renderer;

int SDL_ShishInit() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 0;
    }
    (void)atexit(SDL_Quit);

    SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);
    if(!renderer || !window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize window/renderer: %s", SDL_GetError());
        return 0;
    }

    SDL_SetWindowIcon(window, IMG_Load("data/kuri2d.png"));
    SDL_SetWindowTitle(window, "Kuri 2D");

    return 1;
}

void screenshot(char *name) {
	int n=0;
	char filename[32];
	struct stat buffer;
	for(n=0; n<9999; n++) {
		sprintf(filename, "%s_%04i.bmp", name, n);
		if(stat(filename, &buffer) == -1) break;
	}
	// TODO: SDL_SaveBMP(rootSurface, filename);
}

void toggleFullscreen(SDL_Window* window) {
	Uint32 fullscreenFlag = SDL_WINDOW_FULLSCREEN;
	Uint32 isFullscreen = SDL_GetWindowFlags(window) & fullscreenFlag;
	SDL_SetWindowFullscreen(window, isFullscreen ? 0 : fullscreenFlag);
	SDL_ShowCursor(isFullscreen);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
    SDL_RenderSetLogicalSize(renderer, 640, 480);
}

void drawString(TTF_Font *font, char *text, SDL_Color color, int x, int y) {
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    drawTexture(texture, x, y);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void fillRect(int x, int y, int w, int h, int c) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_SetRenderDrawColor(renderer, c<<24, c<<16, c<<8, c<<0);
    SDL_RenderFillRect(renderer, &rect);
}

SDL_Texture *loadTexture(const char *name) {
    SDL_Surface *surface = IMG_Load(name);
    if(!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load image %s: %s", name, SDL_GetError());
        return 0;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if(!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError());
        return 0;
    }

    SDL_FreeSurface(surface);
    return texture;
}

void drawTexture(SDL_Texture *img, int x, int y) {
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
    SDL_QueryTexture(img, NULL, NULL, &dest.w, &dest.h);
	SDL_RenderCopy(renderer, img, NULL, &dest);
}

void playSound(/*@ unused @*/ int channel, /*@ unused @*/ BasicSound* data) {
	/* FIXME: implement this */
}
