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
#include <unistd.h>
#include "kuri2d.h"

int main(int argc, char *argv[]) {
	int editMode = 0, opt, doWipe = 0, ok = 1;
	char levelName[32];
	Uint32 vidFlags = (Uint32)(SDL_HWSURFACE | SDL_DOUBLEBUF);

	printf("Kuri2d %s - SDL %i.%i.%i \n", VERSION, SDL_MAJOR_VERSION,
	       SDL_MINOR_VERSION, SDL_PATCHLEVEL);

	strncpy(levelName, "default", 32);

	while(ok) {
		opt = getopt(argc, argv, "e:fwhv");
		if(opt == -1) {break;}

		switch(opt) {
			case 'e':
				editMode = 1;
				strncpy(levelName, optarg, 32);
				break;
			case 'f':
				vidFlags |= SDL_FULLSCREEN;
				break;
			case 'w':
				doWipe = 1;
				break;
			case 'h':
				printf("Usage: %s [FLAGS]\n\n", argv[0]);
				printf("  -e [NAME]   - edit level named NAME\n");
				printf("  -f          - fullscreen\n");
				printf("  -w          - wipe highscores\n");
				printf("  -h          - show this help\n");
				printf("  -v          - exit after showing version\n\n");
				ok = 0;
				break;
			case 'v':
				ok = 0;
				break;
		}
	}
	if(!ok) return 0;
	
	if(!doInit(vidFlags, doWipe)) return 1;
	if(!(editMode ? doEditor(levelName) : doKuri2d())) return 2;
	if(!doExit()) return 3;

	return 0;
}

