/**
 * vim:ts=4:sw=4
 * menu.c - (c) Shish 2003 - shishthemoomin@yahoo.com
 * an extension of main.c to handle the menu parts
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include "SDL.h"
#include <math.h>
#include "kuri2d.h"

static void tmp_dumpHiScores(void);

/* Menu Instructions */
static char *mins[] = {
	"Arrows", "move",
	"T", "set / activate trap",
	"B", "detonate bombs",
	"Q", "restart level",
	"Esc", "leave game",
	"P", "toggle pause",
	" ", " ",
	" ", "block; trap for points",
	" ", "bomb; trap and detonate",
	" ", "forbidden; don't trap!",
	NULL
};

/**
 * show the "menu" bitmap
 * wait 500ms
 * clear event stack
 * wait for an event
 */
void doMenu() {
	int half = 640 / 2 - 16;
	int xo2, minsy = 100, i; /* x offset 2, menu instruction y offset, int */
	SDL_Event event;
	SDL_Surface *hitex, *t1, *t2, *t3;
	char histring[64];
	
	drawImage(backgrounds[BG_BACK], 0, 0);
	drawImage(backgrounds[BG_MENU], 0, 96);
	for(i=0; mins[i]; i+=2, minsy += 32) {
		drawImage(t1 = TTF_RenderText_Blended(fonts[FONT_MINS], mins[i],   black), 37,  minsy);
		drawImage(t2 = TTF_RenderText_Blended(fonts[FONT_MINS], "-",       black), 110, minsy);
		drawImage(t3 = TTF_RenderText_Blended(fonts[FONT_MINS], mins[i+1], black), 125, minsy);
		SDL_FreeSurface(t1);
		SDL_FreeSurface(t2);
		SDL_FreeSurface(t3);
	}

	minsy = 180;
	for(i=0; i<MAX_HISCORES; i++, minsy += 32) {
		if(hiscores[i].score > 0) {
			sprintf(histring, "%s, %i (%i)", hiscores[i].name, hiscores[i].score, hiscores[i].level);
			drawImage(hitex = TTF_RenderText_Blended(fonts[FONT_HISCORE], histring, black), 400, minsy);
			SDL_FreeSurface(hitex);
		}
	}

	/* drawImage(texts[TEXT_PRESS], 170, 435); */
	drawImage(backgrounds[BG_LOGO], 198, 14);
	drawImage(blockImages[BT_BLOCK], half, 480-32-8);
	refresh();
	SDL_Delay(500);
	while (SDL_PollEvent(&event));

	inGame = 1;
	while (inGame && !hasQuit) {
		frame++;

		while (SDL_PollEvent(&event)) {
			/*@ -usedef @*/
			if (event.type == SDL_QUIT) {hasQuit = 1;}
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_q:
					case SDLK_ESCAPE:
						hasQuit = 1;
						break;
					case SDLK_d:
						tmp_dumpHiScores();
						break;
					default:
						inGame = 0;
						break;
				}
			}
			/*@ =usedef @*/
		}

		/* FIXME : Bounce some blocks around */
		xo2 = (int)(half+(float)sin((frame/30.0)*(3.1415/2.0))*(half-16));

		drawImageClipped(backgrounds[BG_MENU], xo2-16, 432, xo2-16, 336, 64, 48);
		/* drawImage(texts[TEXT_PRESS], 170, 438); */
		drawImage(blockImages[BT_BLOCK], xo2, 480-32-8);
		refresh();
		SDL_Delay(1000 / 30);
	}
}

static void tmp_dumpHiScores() {
	int i;

	printf(" Name   : Level : Score\n");
	for(i=0; i<5; i++) {
		printf("%8.8s: %5i : %i\n", hiscores[i].name,
			hiscores[i].level, hiscores[i].score);
	}
}


/**
 * show the "you win" bitmap
 * wait 500ms
 * clear event stack
 * wait for an event
 */
void doGameWon() {
	SDL_Event event;

	/* clear the field */
	doBombs();
	eatBlocks();

	drawImage(backgrounds[BG_BACK], 0, 0);
	drawImage(backgrounds[BG_WIN], 32, 192);
	refresh();
	SDL_Delay(500);
	while (SDL_PollEvent(&event));

	inGame = 1;
	frame = 0;
	while (inGame && !hasQuit) {
		frame++;
		while (SDL_PollEvent(&event)) {
			/*@ -usedef @*/
			if (event.type == SDL_QUIT) {hasQuit = 1;}
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE: hasQuit = 1; break;
					default: inGame = 0; break;
				}
			}
			/*@ =usedef @*/
		}

		/* FIXME : Bounce some blocks around */
		refresh();
		SDL_Delay(1000 / 10);

		if(frame == 100) break;
	}
}


/**
 * show the "game over" bitmap
 * wait 500ms
 * clear event stack
 * wait for an event
 */
void doGameLost() {
	SDL_Event event;
	SDL_Surface *hiMsg;
	char hiText[64];
	char name[64] = "";
	int namePos = 0, hiPos = 0, i = 0, needUpdate = 0;

	/* clear the field */
	doBombs();
	eatBlocks();

	/* check for hiscore */
	for(i=0; i<MAX_HISCORES; i++) if(state->score > hiscores[i].score) needUpdate = hiPos = i;


	drawImage(backgrounds[BG_BACK], 0, 0);
	drawImage(backgrounds[BG_LOSE], 32, 192);

	refresh();
	SDL_Delay(500);
	while (SDL_PollEvent(&event));

	inGame = 1;
	frame = 0;
	while (inGame && !hasQuit) {
		frame++;
		while (SDL_PollEvent(&event)) {
			/*@ -usedef @*/
			if (event.type == SDL_QUIT) {hasQuit = 1;}
			if (hiPos && event.type == SDL_KEYDOWN) {
				if(event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z) {
					if(namePos < 8) {
						name[namePos++] = event.key.keysym.sym;
						name[namePos] = 0;
						needUpdate = 1;
					}
				}
				else {
					switch (event.key.keysym.sym) {
						case SDLK_RETURN:
							addHiScore(name, state->score, state->level);
							inGame = 0;
							break;
						case SDLK_BACKSPACE:
							if(namePos > 0) name[--namePos] = 0;
							needUpdate = 1;
							break;
						default:
							break;
					}
				}
			}
			else if(event.type == SDL_KEYDOWN) {
				inGame = 0;
			}
			/*@ =usedef @*/
		}
		
		if(needUpdate) {
			drawImage(backgrounds[BG_BACK], 0, 0);
			drawImage(backgrounds[BG_LOSE], 32, 192);
			sprintf(hiText, "New Hiscore: %i points!", state->score);
			drawImage(hiMsg = TTF_RenderText_Blended(fonts[FONT_SCORE], hiText, black), 100, 300);
			SDL_FreeSurface(hiMsg);
			sprintf(hiText, "Enter Name: %s", name);
			drawImage(hiMsg = TTF_RenderText_Blended(fonts[FONT_SCORE], hiText, black), 100, 332);
			SDL_FreeSurface(hiMsg);
			needUpdate = 0;
		}

		/* FIXME : Bounce some blocks around */
		refresh();
		SDL_Delay(1000 / 10);

		if(frame == 100) break;
	}
}
