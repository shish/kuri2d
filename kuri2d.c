/**
 * vim:ts=4:sw=4
 * kuri2d.c - (c) Shish 2003 - shishthemoomin@yahoo.com
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
	int editMode = 0, opt, doWipe = 0;
	char levelName[32];
	Uint32 vidFlags = (Uint32)(SDL_HWSURFACE | SDL_DOUBLEBUF);

	printf("Launching Kuri2d %s...\n", VERSION);
	strncpy(levelName, "default", 32);

	while(1) {
		opt = getopt(argc, argv, "e:fw");
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
		}
	}

#ifdef __linux__
	printf("linux mode: cd-ing to /usr/share/games/kuri2d/\n");
	if(chdir("/usr/share/games/kuri2d/") != 0) {
		printf("cd-ing failed - the game will probably complain about missing data files\n");
	}
#endif
	
	if(!doInit(vidFlags, doWipe)) return 1;
	if(!(editMode ? doEditor(levelName) : doKuri2d())) return 2;
	if(!doExit()) return 3;

	return 0;
}

