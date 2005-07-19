/**
 * vim:ts=4:sw=4
 * exit.c - (c) Shish 2003 - shishthemoomin@yahoo.com
 * frees resources and exits
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include "kuri2d.h"
#include "SDL_shish.h"
#include "SDL_ttf.h"

int doExit() {
	int i;

	FILE *fp = fopen("data/hiscores.dat", "wb+");
	if(!fp) {
		printf("Couldn't open the hiscore file for writing\n");
	}
	else {
		(void)fwrite(hiscores, sizeof(hiscores), 1, fp);
		(void)fclose(fp);
	}

	for(i=0; i<BG_COUNT; i++) {
		if(backgrounds[i] != NULL) {
			SDL_FreeSurface(backgrounds[i]);
		}
	}

	for(i=0; i<BT_COUNT; i++) {
		if(blockImages[i] != NULL) {
			SDL_FreeSurface(blockImages[i]);
		}
	}
	
	for(i=0; i<TEXT_COUNT; i++) {
		if(texts[i] != NULL) {
			SDL_FreeSurface(texts[i]);
		}
	}

	for(i=0; i<MINS_COUNT; i++) {
		if(mins[i] != NULL) {
			SDL_FreeSurface(mins[i]);
		}
	}

	/*@ -compdestroy -unqualifiedtrans @*/
	/* splint doesn't pick up FreeWAV as free()ing memory */
	/* FIXME: Sound 
	for(i=0; i<SND_COUNT; i++) {
		if(sounds[i] != NULL) {
			if(sounds[i]->data != NULL) {
				SDL_FreeWAV(sounds[i]->data);
			}
			free((void *)sounds[i]);
		}
	}
	*/
	/*@ =compdestroy @*/

	for(i=0; i<FONT_COUNT; i++) {
		if(fonts[i] != NULL) {
			TTF_CloseFont(fonts[i]);
		}
	}
	
	/**
	 * free any pointed to variables / objects. I'm not sure
	 * if this is needed, or whether the OS frees everything
	 * when a program exits, but just in case.
	 */
	free(state);
	free(startState);
	free(level);
	/*@ =unqualifiedtrans @*/

	/* write hiscores */
	/* fwrite(hiscores, sizeof(HiScore), 5, stdout); */

	SDL_Quit();
	TTF_Quit();
	return 1;
}

