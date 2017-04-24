/**
 * vim:ts=4:sw=4
 * menu.c - (c) Shish 2003, 2005 - shish@shish.is-a-geek.net
 * an extension of main.c to handle the menu parts
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "kuri2d.h"

/* Menu Instructions */
static char *mins[] = {
	"Arrows", "move",
	"T", "set / activate trap",
	"B", "detonate bombs",
	"Q", "restart level",
    "P", "toggle pause",
#ifdef __EMSCRIPTEN__
    "", "",
#else
	"Esc", "leave game",
#endif
	"", "",
	" ", "block; trap for points",
	" ", "bomb; trap and detonate",
	" ", "forbidden; don't trap!",
	NULL
};


/**
 *
 */
void initMenu() {
    state->score = 0;
    state->lives = 3;
    state->level = 0;
}
void doMenu() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {hasQuit = K2D_TRUE;}
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_q:
                case SDLK_ESCAPE:
                    hasQuit = K2D_TRUE;
                    break;
                default:
                    setScreen(SCR_GAME);
                    break;
            }
        }
    }

    int half = 640 / 2 - 16;
    int xo2, minsy = 100, i; /* x offset 2, menu instruction y offset, int */
    char histring[64];

    drawTexture(backgrounds[BG_BACK], 0, 0);
    drawTexture(backgrounds[BG_MENU], 0, 96);
    drawTexture(backgrounds[BG_LOGO], 198, 14);

    for(i=0; mins[i]; i+=2, minsy += 32) {
        drawString(fonts[FONT_MINS], mins[i], black, 37, minsy);
        if(strlen(mins[i]) > 0) {
            drawString(fonts[FONT_MINS], "-", black, 110, minsy);
        }
        drawString(fonts[FONT_MINS], mins[i + 1], black, 125, minsy);
    }

    minsy = 180;
    for(i=0; i<MAX_HISCORES; i++, minsy += 32) {
        if(hiscores[i].score > 0) {
            sprintf(histring, "%s, %i (%i)", hiscores[i].name, hiscores[i].score, hiscores[i].level);
            drawString(fonts[FONT_HISCORE], histring, black, 400, minsy);
        }
    }

    xo2 = (int)(half+(float)sin((state->frame/30.0)*(3.1415/2.0))*(half-16));
    drawTexture(blockImages[BT_BLOCK], xo2, 480 - 32 - 8);
}
