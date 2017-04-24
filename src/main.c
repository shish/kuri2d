/**
 * vim:ts=4:sw=4
 * kuri2d.c - (c) Shish 2003, 2005 - shish@shish.is-a-geek.net
 * hack to make the thing work on MinGW without lots of #ifdefs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef VERSION
 #error "VERSION not defined"
#endif
 
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "kuri2d.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


static void doFrame(void *arg);


int main(int argc, char *argv[]) {
	int editMode = 0, opt, doWipe = 0, ok = 1;
	char levelName[LEVEL_LEN];
	Uint8 fullscreen = 0;

	printf("Kuri2d %s - SDL %i.%i.%i \n", VERSION, SDL_MAJOR_VERSION,
	       SDL_MINOR_VERSION, SDL_PATCHLEVEL);

	strncpy(levelName, "default", LEVEL_LEN);

	while(ok) {
		opt = getopt(argc, argv, "e:fwhv");
		if(opt == -1) {break;}

		switch(opt) {
			case 'e':
				editMode = 1;
				strncpy(levelName, optarg, LEVEL_LEN);
				break;
			case 'f':
				fullscreen = 1;
				break;
			case 'w':
				doWipe = 1;
				break;
			case 'v':
				ok = 0;
				break;
			case 'h':
			default:
				printf("Usage: %s [FLAGS]\n\n", argv[0]);
				printf("  -e [NAME]   - edit level named NAME\n");
				printf("  -f          - fullscreen\n");
				printf("  -w          - wipe highscores\n");
				printf("  -h          - show this help\n");
				printf("  -v          - exit after showing version\n\n");
				ok = 0;
				break;
		}
	}
	if(!ok) return 0;
	
	if(!doInit(fullscreen, doWipe)) return 1;
	setScreen(editMode ? SCR_EDIT : SCR_MENU);

#ifdef __EMSCRIPTEN__
	// TODO: By passing an fps argument to emscripten_set_main_loop_arg equal to -1
    // Emscripten will use requestAnimationFrame under the bonnet and so you will
    // not refresh the graphics faster than the browser, in general, it is a good idea to do this.
    emscripten_set_main_loop_arg(doFrame, NULL, 30, 1);
#else
	while (!hasQuit) {
		doFrame(NULL);
		SDL_Delay(1000 / 30);
	}
#endif

	if(!doExit()) return 3;

	return 0;
}

static void doFrame(void *arg) {
    state->frame++;

    SDL_RenderClear(renderer);
    switch(state->screen) {
        case SCR_MENU: doMenu(); break;
        case SCR_GAME: doGame(); break;
        case SCR_LOSE: doLose(); break;
        case SCR_WIN: doWin(); break;
        case SCR_EDIT: doEditor(); break;
    }
    SDL_RenderPresent(renderer);

#ifdef __EMSCRIPTEN__
    // ignore hasQuit
    // if(hasQuit) emscripten_cancel_main_loop();
#endif
}

void setScreen(screen screen) {
    switch(screen) {
        case SCR_MENU: initMenu(); break;
        case SCR_GAME: initGame(); break;
        case SCR_LOSE: initLose(); break;
        case SCR_WIN: initWin(); break;
        case SCR_EDIT: initEditor(); break;
    }
    printf("Switching to screen: %d\n", screen);
    state->screen = screen;
}

