/**
 * vim:ts=4:sw=4
 * init.c - (c) Shish 2003, 2005 - shish@shish.is-a-geek.net
 * loads resources
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "kuri2d.h"

/* static int loadWav(char *name, soundType slot); */
static int loadHiScores(void);
static int loadBackgrounds(void);
static int loadBlocks(void);
static int loadLevels(void);
static int loadSounds(void);
static int loadFonts(void);
static int loadTexts(void);

int doInit(Uint8 fullscreen, int doWipe) {
    SDL_ShishInit();

	/* malloc some defaults - they *should* be reloaded later */
	level = (KuriLevel *)malloc(sizeof(KuriLevel));
	startState = malloc(sizeof(State));
	state = malloc(sizeof(State));
	strcpy(state->name, "Player");

	for(int i=0; i<MAX_HISCORES; i++) {
		strcpy(hiscores[i].name, "nobody");
		hiscores[i].score = 0;
		hiscores[i].level = 0;
	}

	if(level && startState && state &&
		(doWipe ? 1 : loadHiScores()) &&
		loadBackgrounds() &&
		loadBlocks() &&
		loadLevels() &&
		loadSounds() &&
		loadFonts() &&
		loadTexts()) {
		return 1;
	}

	return 0;
}
	
static int loadHiScores() {
	size_t read;
	int i;
	FILE *fp;

	if((fp = fopen("hiscores.dat", "rb"))) {
		read = fread(hiscores, sizeof(hiscores), 1, fp);
		if(read != 1) {
			printf("HiScore reading failed - only %zd of %zd charts were read\n",
					read, sizeof(hiscores));
		}
		(void)fclose(fp);
	}
	else {
		printf("Couldn't open '%s' for reading\n", "hiscores.dat");
		for(i=0; i<MAX_HISCORES; i++) {
			strcpy(hiscores[i].name, "nobody");
			hiscores[i].score = 0;
			hiscores[i].level = 0;
		}
	}
	return 1;
}

#define BKG(NAME, FILE) backgrounds[NAME] = loadTexture(FILE)

static int loadBackgrounds() {
	int i;

	BKG(BG_BACK, "data/menu/back.png");
	BKG(BG_MENU, "data/menu/menu.png");
	BKG(BG_GAME, "data/menu/game.png");
	BKG(BG_WIN, "data/menu/gamewon.png");
	BKG(BG_LOSE, "data/menu/gamelost.png");
	BKG(BG_LOGO, "data/menu/logo.png");
	BKG(BG_PAUSED, "data/menu/paused.png");

	for(i=0; i<BG_COUNT; i++) {
		if(backgrounds[i] == NULL) {
			printf("Background image %i missing: %s\n", i, SDL_GetError());
			return 0;
		}
	}
	return 1;
}

#define IMG(NAME, FILE) blockImages[NAME] = loadTexture(FILE)

static int loadBlocks() {
	int i;
	/* blocks */
    IMG(BT_BLOCK,      "data/blocks/block.png");
	IMG(BT_BLOCK,      "data/blocks/block.png");
	IMG(BT_BOMB,       "data/blocks/green.png");
	IMG(BT_FORBIDDEN,  "data/blocks/black.png");
	/* tiles */
	IMG(BT_EMPTY,      "data/blocks/empty.png");
	IMG(BT_ABOMB,      "data/blocks/dgreen.png");
	IMG(BT_TRIGGERED,  "data/blocks/red.png");
	IMG(BT_TRAP,       "data/blocks/blue.png");
	/* overlays */
	IMG(BT_ABOMB_OVER, "data/marks/greent.png");
	IMG(BT_TRAP_OVER,  "data/marks/bluet.png");
	IMG(BT_TRIG_OVER,  "data/marks/redt.png");
	/* Objects */
	IMG(BT_MARKER,     "data/marks/marker.png");

	for(i=0; i<BT_COUNT; i++) {
		if(blockImages[i] == NULL) {
			printf("Block image %i missing: %s\n", i, SDL_GetError());
			return 0;
		}
	}
	return 1;
}

static int loadSounds() {
	/*
	int i;

	loadWav("data/sound/start.wav", SND_START);
	loadWav("data/sound/set.wav",   SND_SET);
	loadWav("data/sound/trig.wav",  SND_TRIG);
	loadWav("data/sound/drop.wav",  SND_DROP);
	loadWav("data/sound/die.wav",   SND_DIE);
	loadWav("data/sound/lose.wav",  SND_LOSE);
	loadWav("data/sound/win.wav",   SND_WIN);

	for(i=0; i<SND_COUNT; i++) {
		if(sounds[i] == NULL) {
			printf("Sound %i missing\n", i);
			return 0;
		}
	}
	*/
	return 1;
}

static int loadLevels() {
	Uint8 i, j, k;
	FILE *levList = fopen("data/levlist.dat", "r");
	char buf[32];

	if(levList) {
		i = 0;
		while(!feof(levList)) {
			(void)fgets(buf, 32, levList);
			if(strlen(buf)) {
				buf[strlen(buf)-1] = 0;
				strcpy((&levels[i])->name, buf);
				if(readMap(&levels[i])) i++;
			}
		}
		(void)fclose(levList);
	}
	else {
		printf("Couldn't find '%s': generating levels\n", "data/levlist.dat");
		state->level = 0;
		for(k=0; k<16; k++) {
			levels[k].inited = 1;
			if(k <= 5) {
				levels[k].fieldWidth = 5;
				levels[k].fieldHeight = 8;
			}
			else if(k <= 10) {
				levels[k].fieldWidth = 6;
				levels[k].fieldHeight = 10;
			}
			else {
				levels[k].fieldWidth = 7;
				levels[k].fieldHeight = 12;
			}
			levels[k].target = 5;
			for(i=0; i<levels[k].fieldHeight/4; i++) {
				for(j=0; j<levels[k].fieldWidth; j++) {
					levels[k].blocks[j][i] = (blockType)(rand()/(RAND_MAX/3));
				}
			}
			for(i=levels[k].fieldHeight/4; i<levels[k].fieldHeight; i++) {
				for(j=0; j<levels[k].fieldWidth; j++) {
					levels[k].blocks[j][i] = BT_NULL;
				}
			}
		}
	}

	return 1;
}

static int loadFonts() {
	int i;
	TTF_Init();
	(void)atexit(TTF_Quit);

	fonts[FONT_TITLE]   = TTF_OpenFont("data/fonts/arial.ttf", 72);
	fonts[FONT_SCORE]   = TTF_OpenFont("data/fonts/arial.ttf", 26);
	fonts[FONT_MINS]    = TTF_OpenFont("data/fonts/arial.ttf", 20);
	fonts[FONT_HISCORE] = TTF_OpenFont("data/fonts/arial.ttf", 20);

	for(i=0; i<FONT_COUNT; i++) {
		if(fonts[i] == NULL) {
			printf("Font %i missing (%s)\n", i, SDL_GetError());
			return 0;
		}
	}

	black.r = 000; black.g = 000; black.b = 000;
	white.r = 255; white.g = 255; white.b = 255;
	
	return 1;
}

#define REN(fontid, text, col) SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(fonts[fontid], text, col))

static int loadTexts() {
	int i;
	
	texts[TEXT_TITLE]  = REN(FONT_TITLE, "Kuri2d", black);
	texts[TEXT_SCORE]  = REN(FONT_SCORE, "Score", black);
	texts[TEXT_TARGET] = REN(FONT_SCORE, "Target", black);
	texts[TEXT_LIVES]  = REN(FONT_SCORE, "Lives", black);
	texts[TEXT_LEVEL]  = REN(FONT_SCORE, "Level", black);
	texts[TEXT_PRESS]  = REN(FONT_SCORE, "Press Any Key To Play", white);
	texts[TEXT_EDHELP] = REN(FONT_SCORE, "<-edhelp here>", black);

	for(i=0; i<TEXT_PRECOUNT; i++) {
		if(texts[i] == NULL) {
			printf("Text %i failed rendering (%s)\n", i, SDL_GetError());
			return 0;
		}
	}
	return 1;
}


/*
static int loadWav(char *name, soundType slot) {
	printf("Loding wav %s\n", name);
	sounds[slot] = malloc(sizeof(BasicSound));
	SDL_LoadWAV(
		name,
		sounds[slot]->sound,
		&sounds[slot]->data,
		&sounds[slot]->length);
	return 0;
}
*/
