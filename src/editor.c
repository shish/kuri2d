/**
 * vim:ts=4:sw=4
 * kurried.c - (c) Shish 2003, 2005 - shish@shish.is-a-geek.net
 * edits kuri2d levels
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
#include "kuri2d.h"
#include "SDL_shish.h"

static void setBlock(blockType type);
static void renameMap(void);
static char *mapSig = "KM01";

void initEditor(char *fname) {
	printf("Kuri2d Editor initing...\n");

	setOffsets();

	/* set the level's name, then reload it */
	strcpy(level->name, fname);
	if(!readMap(level)) {
		printf("Load of map %s failed\n", fname);
	}
}

void doEditor() {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_QUIT) {hasQuit = K2D_TRUE;}
		if(event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE: hasQuit = K2D_TRUE; break;
				case SDLK_UP:     state->py--; break;
				case SDLK_DOWN:   state->py++; break;
				case SDLK_LEFT:   state->px--; break;
				case SDLK_RIGHT:  state->px++; break;
				case SDLK_n: renameMap(); break;
				case SDLK_d: setBlock(BT_NULL);      break;
				case SDLK_p: setBlock(BT_BLOCK);     break;
				case SDLK_b: setBlock(BT_BOMB);      break;
				case SDLK_f: setBlock(BT_FORBIDDEN); break;
				case SDLK_KP_0:     level->fieldWidth--;  break;
				case SDLK_KP_ENTER: level->fieldWidth++;  break;
				case SDLK_KP_MINUS: level->fieldHeight--; break;
				case SDLK_KP_PLUS:  level->fieldHeight++; break;
				case SDLK_r: (void)readMap(level);  break;
				case SDLK_w: (void)writeMap(level); break;
				default: break;
			}
		}
	}

	drawTexture(backgrounds[BG_BACK], 0, 0);
	drawTexture(backgrounds[BG_GAME], 6, 6);
	drawTexture(backgrounds[BG_LOGO], 300, 14);
	drawEdit();
}

static void setBlock(blockType type) {
	if(state->px < level->fieldWidth && state->py < level->fieldHeight) {
		level->blocks[state->px][state->py] = type;
	}
}

static void renameMap() {
	printf("Type the level name: ");
	if(fgets(level->name, 16, stdin)) {
		/* cut the \n */
		level->name[strlen(level->name)-1] = '\00';
	}
}

int readMap(KuriLevel *klevel) {
	FILE *mapfile;
	char *fname = malloc(FILENAME_MAX);
	
	snprintf(fname, FILENAME_MAX, "maps/%s.kmp", klevel->name);

	mapfile = fopen(fname, "rb");
	if(mapfile) {
		printf("Reading file '%s'\n", fname);
		(void)fread(klevel, sizeof(KuriLevel), 1, mapfile);
		(void)fclose(mapfile);
		if(strncmp(klevel->sig, mapSig, 4) != 0) {
			printf("WARNING: Map '%s' has bad header ('%4.4s', not '%s')\n",
				fname, klevel->sig, mapSig);
		}
	}
	else {
		printf("File '%s' not found\n", fname);
	}
	free(fname);

	return (mapfile != NULL);
}

int writeMap(KuriLevel *klevel) {
	FILE *mapfile;
	char fname[128];
	
	snprintf(fname, 128, "maps/%s.kmp", klevel->name);
	
	if(!klevel) {
		printf("WARNING: Tried to save NULL!\n");
		return 0;
	}
	strcpy(klevel->sig, mapSig); /* set the sig to the current version */
	klevel->inited = (Uint8)1;   /* this should be set for all maps */
	printf("Writing file '%s'\n", fname);

	mapfile = fopen(fname, "wb+");
	if(mapfile) {
		(void)fwrite(klevel, sizeof(KuriLevel), 1, mapfile);
		(void)fclose(mapfile);
	}
	else {
		printf("File '%s' could not be written to\n", fname);
		return 0;
	}
	return 1;
}
