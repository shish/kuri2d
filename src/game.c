/**
 * vim:ts=4:sw=4
 * main.c - (c) Shish 2003, 2005 - shish@shish.is-a-geek.net
 * controls most of the GUI IO
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "kuri2d.h"

SDL_Texture *backgrounds[BG_COUNT];
SDL_Texture *blockImages[BT_COUNT];
SDL_Texture *texts[TEXT_COUNT];
BasicSound  *sounds[SND_COUNT];
TTF_Font	*fonts[FONT_COUNT];
SDL_Color    black, white;

HiScore hiscores[MAX_HISCORES];
k2d_boolean hasQuit = K2D_FALSE;
State *state, *startState;
KuriLevel levels[MAX_LEVELS];
KuriLevel *level;

static Uint32 x, y;
static Uint32 ox=32, oy=32;
static k2d_boolean paused = K2D_FALSE;

static SDL_Texture *getBlockOverlay(blockType type);

void initGame() {
    state->level++;
    if(!levels[state->level].inited) { /* do this after the menu check */
        printf("Out of levels, starting again from level 0!\n");
        state->level = 0;
    }

	memcpy(level, &levels[state->level], sizeof(KuriLevel));
	resetField();
	setOffsets();

	state->px = level->fieldWidth / 2;
    state->py = level->fieldHeight - 2;

	/* backup the state at the start of each round */
	memcpy(startState, state, sizeof(State));

    state->frame = 0;
    playSound(CH_START, sounds[SND_START]);
}

/**
 * play "start" sound
 * get input
 * draw field
 */
void doGame() {
	SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {hasQuit = K2D_TRUE;}
        if(event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym) {
                case SDLK_ESCAPE: hasQuit = K2D_TRUE; break;
                case SDLK_UP:     tryUp();     break;
                case SDLK_DOWN:   tryDown();   break;
                case SDLK_LEFT:   tryLeft();   break;
                case SDLK_RIGHT:  tryRight();  break;
                case SDLK_b:      doBombs();   break;
                case SDLK_t:      doTrap();    break;
                case SDLK_q:      setScreen(SCR_MENU); break;
                case SDLK_p: paused = !paused; break;
                case SDLK_PRINTSCREEN:  screenshot("kuri"); break;
                case SDLK_F11: toggleFullscreen(window);  break;

                default: break;
            }
        }
    }

    drawGame();

    if(paused) {
        state->frame--;  // don't move things while paused
        drawTexture(backgrounds[BG_PAUSED], 0, 0);
    }

    if(!paused && (state->frame % 32 == 31)) {
        if(isStageClear()) setScreen(SCR_WIN);
        slide();	/* slide it before it resets */
    }
}

void kill() {
    state->lives--;
    if(state->lives > 0) {
        memcpy(state, startState, sizeof(State)); /* reset game to start of level */
        initGame();
        playSound(CH_END, sounds[SND_DIE]);
    }
    else {
        setScreen(SCR_LOSE);
        playSound(CH_END, sounds[SND_DIE]);
    }
}

/**
 * set ox and oy - the offsets for drawing blocks at
 */
void setOffsets() {
	ox = (128+16-3)-(level->fieldWidth*32)/2;
	oy = 32;
	/* printf("Offset set to (%i,%i)\n", (int)ox, (int)oy); */
}

/* don't waste cycles, only draw the whole thing every 32
 * frames -- woah! 'if'-ing this ~doubles the game speed! */
static void drawBack() {
    drawTexture(backgrounds[BG_BACK], 0, 0);
    drawTexture(backgrounds[BG_GAME], 6, 6);
    drawTexture(backgrounds[BG_LOGO], 300, 14);
}
static void drawField() {
	/* field */
	for (y = 0; y < level->fieldHeight; y++) {
		for (x = 0; x < level->fieldWidth; x++) {
            drawTexture(blockImages[(int) level->field[x][y]],
                        ox + (x * 32), oy + (y * 32));
		}
	}
}

/* blocks */
/*
 * (frame%16)*2 is smooth, we want steps.
 * sin(1) is 18283238232 (OWTTA), WTF?!?!
 * (float)sin(1) is 0.841471
 *
 * Also note: ANSI C lacks math constants, no pi for me :(
 */
static void drawBlocks() {
	/* This function was a pain in the arse to do, don't mess with it! */
	float yoff = (((float) sin(((state->frame % 32) / 32.0) * (3.14159 / 2))) * 32.0);
	for (y = 0; y < level->fieldHeight; y++) {
		for (x = 0; x < level->fieldWidth; x++) {
            drawTexture(blockImages[level->blocks[x][y]],
                        ox + (x * 32), oy + (y * 32) + (int) yoff);
		}
	}

	for (y = 0; y < level->fieldHeight; y++) {
		for (x = 0; x < level->fieldWidth; x++) {
			if(level->field[x][y] > 4 && level->field[x][y] <= 6) {
                drawTexture(getBlockOverlay(level->field[x][y]),
                            ox + (x * 32), oy + (y * 32));
			}
		}
	}

	/* black out the end of the level */
	fillRect(ox, oy + level->fieldHeight*32,
		level->fieldWidth*32, (13-level->fieldHeight)*32, 0xFF000000);
}

static void drawScores() {
	char scoret[64], livest[64], levelt[64], targett[64];
	int y=100, a=25;
	/* drawTexture(backgrounds[BG_SCORE], 300, 125); */

    sprintf(levelt, "%i", state->level);
    sprintf(targett, "%i", level->target);
    sprintf(scoret, "%i", state->score);
	sprintf(livest, "%i", state->lives);
	
	/* Slooooow */
    drawTexture(texts[TEXT_LEVEL], 300, y += a);
    drawString(fonts[FONT_SCORE], levelt, black, 320, y += a);
	
	y += a;
    drawTexture(texts[TEXT_SCORE], 300, y += a);
    drawString(fonts[FONT_SCORE], scoret, black, 320, y += a);

	y += a;
    drawTexture(texts[TEXT_TARGET], 300, y += a);
    drawString(fonts[FONT_SCORE], targett, black, 320, y += a);
	
	y += a;
    drawTexture(texts[TEXT_LIVES], 300, y += a);
    drawString(fonts[FONT_SCORE], livest, black, 320, y += a);
}
static void drawEditHelp() {
    drawTexture(texts[TEXT_EDHELP], 300, 125);
}

/**
 * Draw the background
 * Draw the blocks
 * (Draw the scores)
 */
void drawGame() {
    drawBack();
    drawScores();
	drawField();
	fillRect(ox + (state->px * 32), oy + (state->py * 32), 32, 32, 0xFFFF7700);	/* player */
	drawBlocks();
}
void drawEdit() {
	drawBack();
	drawField();
	drawBlocks();
    drawTexture(blockImages[BT_MARKER], ox + (state->px * 32), oy + (state->py * 32));
	drawEditHelp();
}

/**
 * get any overlays, eg the arrows above bombs
 */
static SDL_Texture *getBlockOverlay(blockType type) {
    SDL_Texture *img = NULL;

	switch (type) {
		case BT_ABOMB:     img = blockImages[BT_ABOMB_OVER]; break;
		case BT_TRAP:      img = blockImages[BT_TRAP_OVER];  break;
		case BT_TRIGGERED: img = blockImages[BT_TRIG_OVER];  break;
		default:           break;
	}

	return img;
}
