/**
 * vim:ts=4:sw=4
 * kuri2d.h - (c) Shish 2003, 2005 - shish@shish.is-a-geek.net
 * global include file
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef _KURI2D_H_
#define _KURI2D_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "SDL_shish.h"

typedef enum { K2D_FALSE = 0, K2D_TRUE = 1 } k2d_boolean;

/*
=============
top level
=============
*/
#define MAX_LEVELS 128
#define MAX_HISCORES 5
#define NAME_LEN 8
#define LEVEL_LEN 32

int doInit(Uint8 fullscreen, int doWipe);
int doExit();

typedef enum screen {
    SCR_MENU,
    SCR_GAME,
    SCR_WIN,
    SCR_LOSE,
	SCR_EDIT,
} screen;

typedef enum background {
	BG_BACK,
	BG_MENU,
	BG_GAME,
	BG_WIN,
	BG_LOSE,
	BG_LOGO,
	BG_PAUSED,
	BG_COUNT
} backgroundId;

typedef enum blockType {
	/* blocks */
	BT_BLOCK,
	BT_BOMB,
	BT_FORBIDDEN,
	/* tiles */
	BT_EMPTY,
	BT_ABOMB,
	BT_TRAP,
	BT_TRIGGERED,
	/* overlays */
	BT_ABOMB_OVER,
	BT_TRAP_OVER,
	BT_TRIG_OVER,
	/* objects */
	BT_MARKER,
	/* count */
	BT_COUNT,
	BT_NULL
} blockType;

typedef enum soundType {
	SND_START,
	SND_SET,
	SND_TRIG,
	SND_DROP,
	SND_DIE,
	SND_LOSE,
	SND_WIN,
	SND_COUNT
} soundType;

typedef enum channels {
	CH_START,
	CH_BGM,
	CH_DROP,
	CH_END
} channelId;

typedef enum fonts {
	FONT_TITLE,
	FONT_SCORE,
	FONT_MINS,
	FONT_HISCORE,
	FONT_COUNT
} fontType;

typedef enum texts {
	/* set texts */
	TEXT_TITLE,
	TEXT_SCORE,
	TEXT_TARGET,
	TEXT_LIVES,
	TEXT_LEVEL,
	TEXT_PRESS,
	TEXT_EDHELP,
	TEXT_PRECOUNT,
	TEXT_COUNT
} textId;

typedef struct KuriLevel {
	char  sig[4];
	Uint8 inited;                  /* whether or not the map is ready */
	char  name[16], hint[128];     /* for on screen display */
	Uint8 fieldWidth, fieldHeight; /* size of the playing field */
	blockType blocks[16][64];      /* blocks outside the field = null */
	blockType field[16][64];	   /* only set for special levels */
	Uint8 target, time;            /* block paces / seconds to beat */
} KuriLevel;

typedef struct HiScore {
	char name[NAME_LEN];
	int score, level;
} HiScore;

typedef struct State {
    int screen;
    int frame;
	char name[NAME_LEN];
	int score, lives, level;
    Uint8 px, py;
} State;


/*
=============
main.c
=============
*/
void setScreen(screen screen);

/*
=============
game.c
=============
*/
extern SDL_Texture *backgrounds[BG_COUNT];
extern SDL_Texture *blockImages[BT_COUNT];
extern SDL_Texture *texts[TEXT_COUNT];
extern BasicSound  *sounds[SND_COUNT];
extern TTF_Font    *fonts[FONT_COUNT];

void initGame(void);
void doGame(void);
void drawGame(void);
void drawEdit(void);
void setOffsets(void);
void kill(void);

extern HiScore     hiscores[MAX_HISCORES];
extern State      *state, *startState;
extern SDL_Color   black, white;
extern k2d_boolean hasQuit;
extern KuriLevel   levels[MAX_LEVELS];
extern KuriLevel  *level;

/*
=============
editor.c
=============
*/
void initEditor();
void doEditor();

int readMap(KuriLevel *level);
int writeMap(KuriLevel *level);

/*
=============
logic.c
=============
*/
void tryUp(void);
void tryDown(void);
void tryLeft(void);
void tryRight(void);
void doTrap(void);
void doBombs(void);
void eatBlocks(void);
void slide(void);
void resetField(void);
int  isStageClear(void);

/*
=============
menu.c
=============
*/
void initMenu(void);
void doMenu(void);

/*
=============
win.c
=============
*/
void initWin(void);
void doWin(void);

/*
=============
lose.c
=============
*/
void initLose(void);
void doLose(void);

void addHiScore(char *name, int score, int level);

#endif /* _KURI2D_H_ */
