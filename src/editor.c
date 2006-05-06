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
#include "binreloc.h"
#include "SDL_shish.h"

static void setBlock(blockType type);
static void renameMap(void);
static char *mapSig = "KM01";

int doEditor(char *fname) {
	printf("Kuri2d Editor initing...\n");

	setOffsets();
	drawImage(backgrounds[BG_BACK], 0, 0);
	drawImage(backgrounds[BG_GAME], 6, 6);
	drawImage(backgrounds[BG_LOGO], 300, 14);

	/* set the level's name, then reload it */
	/*@-mayaliasunique@*/
	strcpy(level->name, fname);
	/*@=mayaliasunique@*/
	if(!readMap(level)) {
		printf("Load of map %s failed\n", fname);
	}

	/*@-usedef@*/
	while(hasQuit == 0) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {hasQuit = 1;}
			if(event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE: hasQuit = 1; break;
					case SDLK_UP:     py--; break;
					case SDLK_DOWN:   py++; break;
					case SDLK_LEFT:   px--; break;
					case SDLK_RIGHT:  px++; break;
					case SDLK_n: renameMap(); break;
					case SDLK_d: setBlock(BT_NULL);      break;
					case SDLK_p: setBlock(BT_BLOCK);     break;
					case SDLK_b: setBlock(BT_BOMB);      break;
					case SDLK_f: setBlock(BT_FORBIDDEN); break;
					case SDLK_KP0:      level->fieldWidth--;  break;
					case SDLK_KP_ENTER: level->fieldWidth++;  break;
					case SDLK_KP_MINUS: level->fieldHeight--; break;
					case SDLK_KP_PLUS:  level->fieldHeight++; break;
					case SDLK_r: (void)readMap(level);  break;
					case SDLK_w: (void)writeMap(level); break;
					default: break;
				}
			}
		}
		drawEdit();
		
		SDL_Delay(1000/10);
	}
	/*@=usedef@*/

	return 1;
}

static void setBlock(blockType type) {
	if(px < level->fieldWidth && py < level->fieldHeight) {
		level->blocks[px][py] = type;
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

	mapfile = fopen(br_find_data(fname), "rb");
	if(mapfile) {
		printf("Reading file '%s'\n", br_find_data(fname));
		(void)fread(klevel, sizeof(KuriLevel), 1, mapfile);
		(void)fclose(mapfile);
		if(strncmp(klevel->sig, mapSig, 4) != 0) {
			printf("WARNING: Map '%s' has bad header ('%4.4s', not '%s')\n",
				fname, klevel->sig, mapSig);
		}
	}
	else {
		printf("File '%s' not found\n", br_find_data(fname));
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
	printf("Writing file '%s'\n", br_find_data(fname));

	mapfile = fopen(br_find_data(fname), "wb+");
	if(mapfile) {
		(void)fwrite(klevel, sizeof(KuriLevel), 1, mapfile);
		(void)fclose(mapfile);
	}
	else {
		printf("File '%s' could not be written to\n", br_find_data(fname));
		return 0;
	}
	return 1;
}

