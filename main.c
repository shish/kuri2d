/**
 * vim:ts=4:sw=4
 * main.c - (c) Shish 2003 - shishthemoomin@yahoo.com
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
#include <stdlib.h>
#include <math.h>
#include "SDL.h"
#include "kuri2d.h"
#include "SDL_shish.h"
#include "SDL_ttf.h"

SDL_Surface *backgrounds[BG_COUNT];
SDL_Surface *blockImages[BT_COUNT];
SDL_Surface *texts[TEXT_COUNT];
BasicSound  *sounds[SND_COUNT];
TTF_Font	*fonts[FONT_COUNT];
SDL_Color    black, white;

HiScore hiscores[MAX_HISCORES];
SDL_bool inGame = SDL_FALSE, hasWon = SDL_FALSE,
         hasLost = SDL_FALSE, hasQuit = SDL_FALSE;
State *state, *startState;
Uint8 px = 3, py = 7;	/* player location */
KuriLevel levels[MAX_LEVELS];
KuriLevel *level;
Uint32 frame = 0;

static Uint32 x, y;
static Uint32 ox=32, oy=32;
static Uint8  frame32;
static SDL_bool paused = SDL_FALSE, pauseDone = SDL_FALSE;

static void initGame(void);
static void doGame(void);
static SDL_Surface *getBlockOverlay(blockType type);

/**
 * The main function
 */
int doKuri2d() {
	while (!hasQuit) {
		if(state->level == 0) {
			state->score = 0;
			state->lives = 1;
			doMenu();
			state->level++;
		}
		if(!levels[state->level].inited) { /* do this after the menu check */
			printf("Out of levels, starting again from level 0!\n");
			state->level = 0;
		}
		
		doGame();
		if (hasLost) {
			doGameLost();
			state->level = 0;
			hasQuit = 0;
		}
		else if(hasWon) {
			doGameWon();
			state->level++;
			hasQuit = 0;
		}
	}

	return 1;
}

static void initGame() {
	char levelt[64], targett[64];

	setField();
	setOffsets();

	px = level->px;
	py = level->py;
	
	/* create the "level" sprite */
	sprintf(levelt, "%i", state->level);
	sprintf(targett, "%i", level->target);
	if(texts[DTEX_LEVEL]  != NULL) SDL_FreeSurface(texts[DTEX_LEVEL]);
	if(texts[DTEX_TARGET] != NULL) SDL_FreeSurface(texts[DTEX_TARGET]);
	texts[DTEX_LEVEL] = TTF_RenderText_Blended(fonts[FONT_SCORE], levelt, black);
	texts[DTEX_TARGET] = TTF_RenderText_Blended(fonts[FONT_SCORE], targett, black);

	/* backup the state at the start of each round */
	memcpy(startState, state, sizeof(State));
}

/**
 * play "start" sound
 * get input
 * draw field
 */
static void doGame() {
	SDL_Event event;
	inGame = 1;
	hasLost = 0;
	hasWon = 0;

	initGame();

	frame = 0;
	playSound(CH_START, sounds[SND_START]);
	while(inGame && !hasLost && !hasWon && !hasQuit) {
		if(!paused) {
			frame++;
			frame32 = frame % 32;
		}

		while(SDL_PollEvent(&event)) {
			/*@ -usedef @*/
			if(event.type == SDL_QUIT) {hasQuit = 1;}
			if(event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
					case SDLK_ESCAPE: hasQuit = 1; break;
					case SDLK_UP:     tryUp();     break;
					case SDLK_DOWN:   tryDown();   break;
					case SDLK_LEFT:   tryLeft();   break;
					case SDLK_RIGHT:  tryRight();  break;
					case SDLK_b:      doBombs();   break;
					case SDLK_t:      doTrap();    break;
					case SDLK_q:      hasLost = 1; break;
					case SDLK_p: paused = !paused; break;
					case SDLK_PRINT:  screenshot("kuri"); break;
                    case SDLK_F11: SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());  break;

					default: break;
				}
			}
			/*@ =usedef @*/
		}

		if(paused) {
			if(!pauseDone) {
				drawImage(backgrounds[BG_PAUSED], 0, 0);
				pauseDone = 1;
			}
			else {
				SDL_Delay(1000/4);
			}
			refresh();
		}
		else {
			drawGame();
		}

		if(!paused && frame32 == 31) {
			if(isStageClear()) hasWon = 1;
			slide();	/* slide it before it resets */
		}

		if(hasLost) {
			state->lives--;
			if(state->lives > 0) {
				hasLost = 0;
				memcpy(state, startState, sizeof(State)); /* reset game to start of level */
				initGame();
			}
		}

		/* if(frame32 == 31) eatBlocks(); */

		SDL_Delay(1000 / 32);
	}
	inGame = 0;
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
	drawImage(backgrounds[BG_BACK], 0, 0);
	drawImage(backgrounds[BG_GAME], 6, 6);
	drawImage(backgrounds[BG_LOGO], 300, 14);
	pauseDone = 0;
}
static void drawField() {
	/* field */
	for (y = 0; y < level->fieldHeight; y++) {
		for (x = 0; x < level->fieldWidth; x++) {
			drawImage(blockImages[(int)level->field[x][y]],
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
	float yoff = (((float) sin((frame32 / 32.0) * (3.14159 / 2))) * 32.0);
	for (y = 0; y < level->fieldHeight; y++) {
		for (x = 0; x < level->fieldWidth; x++) {
			drawImage(blockImages[level->blocks[x][y]],
					ox + (x * 32), oy + (y * 32) + (int) yoff);
		}
	}

	for (y = 0; y < level->fieldHeight; y++) {
		for (x = 0; x < level->fieldWidth; x++) {
			if(level->field[x][y] > 4 && level->field[x][y] <= 6) {
				drawImage(getBlockOverlay(level->field[x][y]),
						ox + (x * 32), oy + (y * 32));
			}
		}
	}

	/* black out the end of the level */
	fillRect(ox, oy + level->fieldHeight*32,
		level->fieldWidth*32, (13-level->fieldHeight)*32, 0xFF000000);
}

static void drawScores() {
	char scoret[64], livest[64];
	int y=100, a=25;
	SDL_Surface *tmp = NULL;
	/* drawImage(backgrounds[BG_SCORE], 300, 125); */

	sprintf(scoret, "%i", state->score);
	sprintf(livest, "%i", state->lives);
	
	/* Slooooow */
	drawImage(texts[TEXT_LEVEL], 300, y += a);
	drawImage(texts[DTEX_LEVEL], 320, y += a);
	
	y += a;
	drawImage(texts[TEXT_SCORE],  300, y += a);
	drawImage(tmp = TTF_RenderText_Blended(fonts[FONT_SCORE],
				scoret, black), 320, y += a);
	SDL_FreeSurface(tmp);
	
	y += a;
	drawImage(texts[TEXT_TARGET], 300, y += a);
	drawImage(texts[DTEX_TARGET], 320, y += a);
	
	y += a;
	drawImage(texts[TEXT_LIVES],  300, y += a);
	drawImage(tmp = TTF_RenderText_Blended(fonts[FONT_SCORE],
				livest, black), 320, y += a);
	SDL_FreeSurface(tmp);
}
static void drawEditHelp() {
	drawImage(texts[TEXT_EDHELP], 300, 125);
}

/**
 * Draw the background
 * Draw the blocks
 * (Draw the scores)
 */
void drawGame() {
	frame32 = frame % 32;

	/* Pray */
	if(pauseDone || frame32 - 1 == 0) {
		drawBack();
		drawScores();
	}
	drawField();
	fillRect(ox + (px * 32), oy + (py * 32), 32, 32, 0xFFFF7700);	/* player */
	drawBlocks();

	refresh();
}
void drawEdit() {
	frame32 = frame % 32;

	if(pauseDone || frame32 - 1 == 0) drawBack();
	drawField();
	drawBlocks();
	drawImage(blockImages[BT_MARKER], ox+(px*32), oy+(py*32));
	drawEditHelp();

	refresh();
}

/**
 * get any overlays, eg the arrows above bombs
 */
static SDL_Surface *getBlockOverlay(blockType type) {
	SDL_Surface *img = NULL;

	switch (type) {
		case BT_ABOMB:     img = blockImages[BT_ABOMB_OVER]; break;
		case BT_TRAP:      img = blockImages[BT_TRAP_OVER];  break;
		case BT_TRIGGERED: img = blockImages[BT_TRIG_OVER];  break;
		default:           break;
	}

	return img;
}
