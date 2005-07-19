/**
 * vim:ts=4:sw=4
 * init.c - (c) Shish 2003 - shishthemoomin@yahoo.com
 * loads resources
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SDL.h"
#include "kuri2d.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

/* static int loadWav(char *name, soundType slot); */
static int loadHiScores(void);
static int loadBackgrounds(void);
static int loadBlocks(void);
static int loadLevels(void);
static int loadSounds(void);
static int loadFonts(void);
static int loadTexts(void);
static int openFrame(Uint32 vidFlags);

int doInit(Uint32 vidFlags, int doWipe) {
	int i;

	if (SDL_Init((Uint32)(SDL_INIT_VIDEO | SDL_INIT_AUDIO))) {
		printf("Unable to init SDL: %s\n", SDL_GetError());
		return 0;
	}
	(void)atexit(SDL_Quit);
	(void)atexit(TTF_Quit);

	/* malloc some defaults - they *should* be reloaded later */
	level = (KuriLevel *)malloc(sizeof(KuriLevel));
	startState = malloc(sizeof(State));
	state = malloc(sizeof(State));
	strcpy(state->name, "Player");
	state->level = (Uint8)0;
	state->score = (Uint8)0;
	state->lives = (Uint8)3;

	if(level && startState && state &&
		loadHiScores() &&
		loadBackgrounds() &&
		loadBlocks() &&
		loadLevels() &&
		loadSounds() &&
		loadFonts() &&
		loadTexts() &&
		openFrame(vidFlags)) {
		return 1;
	}

	if(doWipe) {
		for(i=0; i<MAX_HISCORES; i++) {
			strcpy(hiscores[i].name, "nobody");
			hiscores[i].score = 0;
			hiscores[i].level = 0;
		}
	}
	
	/* just a test */
	/* printf("Inited OK, daemonizing...\n");
	daemon(1, 0); */
	
	return 0;
}
	
static int loadHiScores() {
	int read, i;
	FILE *hs_fp = fopen("data/hiscores.dat", "rb");

	if(!hs_fp) {
		printf("Couldn't open data/hiscores.dat for reading\n");
		for(i=0; i<MAX_HISCORES; i++) {
			strcpy(hiscores[i].name, "nobody");
			hiscores[i].score = 0;
			hiscores[i].level = 0;
		}
	}
	else {
		read = fread(hiscores, sizeof(hiscores), 1, hs_fp);
		if(read != 1) {
			printf("HiScore reading failed - only %i of %i charts were read\n",
					read, sizeof(hiscores));
		}
		(void)fclose(hs_fp);
	}
	return 1;
}

static int loadBackgrounds() {
	int i;

	backgrounds[BG_BACK]   = IMG_Load("data/menu/back.png");
	backgrounds[BG_MENU]   = IMG_Load("data/menu/menu.png");
	backgrounds[BG_GAME]   = IMG_Load("data/menu/game.png");
	backgrounds[BG_WIN]    = IMG_Load("data/menu/gamewon.png");
	backgrounds[BG_LOSE]   = IMG_Load("data/menu/gamelost.png");
	backgrounds[BG_LOGO]   = IMG_Load("data/menu/logo.png");
	backgrounds[BG_PAUSED] = IMG_Load("data/menu/paused.png");

	for(i=0; i<BG_COUNT; i++) {
		if(backgrounds[i] == NULL) {
			printf("Background image %i missing\n", i);
			return 0;
		}
	}
	return 1;
}

static int loadBlocks() {
	int i;
	/* blocks */
	blockImages[BT_BLOCK]      = IMG_Load("data/blocks/block.png");
	blockImages[BT_BOMB]       = IMG_Load("data/blocks/green.png");
	blockImages[BT_FORBIDDEN]  = IMG_Load("data/blocks/black.png");
	/* tiles */
	blockImages[BT_EMPTY]      = IMG_Load("data/blocks/empty.png");
	blockImages[BT_ABOMB]      = IMG_Load("data/blocks/dgreen.png");
	blockImages[BT_TRIGGERED]  = IMG_Load("data/blocks/red.png");
	blockImages[BT_TRAP]       = IMG_Load("data/blocks/blue.png");
	/* overlays */
	blockImages[BT_ABOMB_OVER] = IMG_Load("data/marks/greent.png");
	blockImages[BT_TRAP_OVER]  = IMG_Load("data/marks/bluet.png");
	blockImages[BT_TRIG_OVER]  = IMG_Load("data/marks/redt.png");
	/* Objects */
	blockImages[BT_MARKER]     = IMG_Load("data/marks/marker.png");

	for(i=0; i<BT_COUNT; i++) {
		if(blockImages[i] == NULL) {
			printf("Block image %i missing\n", i);
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
		printf("Found levlist.dat: reading levels\n");
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
		printf("Couldn't find levlist.dat: generating levels\n");
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
			levels[k].px = levels[k].fieldWidth / 2;
			levels[k].py = levels[k].fieldHeight - 2;
			for(i=0; i<levels[k].fieldHeight/4; i++) {
				for(j=0; j<levels[k].fieldWidth; j++) {
					levels[k].blocks[j][i] = rand()/(RAND_MAX/3);
				}
			}
			for(i=levels[k].fieldHeight/4; i<levels[k].fieldHeight; i++) {
				for(j=0; j<levels[k].fieldWidth; j++) {
					levels[k].blocks[j][i] = BT_NULL;
				}
			}
		}
	}

	setField();
	return 1;
}

static int openFrame(Uint32 vidFlags) {
	SDL_Surface *root;

	SDL_WM_SetCaption("Kuri 2d", "Kuri 2d");
	SDL_WM_SetIcon(IMG_Load("data/kuri.png"), NULL);
	root = SDL_SetVideoMode(640, 480, 32, vidFlags);
	if(root == NULL) {
		printf("Unable to open window: %s\n", SDL_GetError());
		return 0;
	}
	setCurrentSurface(root);
	setRootSurface(root);
	return 1;
}

static int loadFonts() {
	int i;
	TTF_Init();

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

#define REN(fontid, text, col) TTF_RenderText_Blended(fonts[fontid], text, col)

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
