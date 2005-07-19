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

#include "SDL.h"
#include "SDL_shish.h"
#include "SDL_ttf.h"

/*
=============
top level
=============
*/
#define MAX_LEVELS 128
#define MAX_HISCORES 5

int doInit(Uint32 vidFlags, int doWipe);
int doKuri2d();
int doEditor(char *fname);
int doExit();

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
	/* dynamic texts */
	DTEX_LEVEL,
	DTEX_TARGET,
	TEXT_COUNT
} textId;

typedef struct KuriLevel {
	char  sig[4];
	Uint8 inited;                  /* whether or not the map is ready */
	char  name[16], hint[128];     /* for on screen display */
	Uint8 fieldWidth, fieldHeight; /* size of the playing field */
	blockType blocks[16][64];      /* blocks outside the field = null */
	blockType field[16][64];	   /* only set for special levels */
	Uint8 px, py;                  /* where the player starts */
	Uint8 target, time;            /* block paces / seconds to beat */
	char  reserved[4096-2203];     /* pad to 4096 for compatibility */
} KuriLevel;

typedef struct HiScore {
	char name[8];
	int score, level;
} HiScore;

typedef struct State {
	char name[8];
	int score, lives, level;
} State;

/*
=============
main.c
=============
*/
extern SDL_Surface *backgrounds[BG_COUNT];
extern SDL_Surface *blockImages[BT_COUNT];
extern SDL_Surface *texts[TEXT_COUNT];
extern BasicSound  *sounds[SND_COUNT];
extern TTF_Font    *fonts[FONT_COUNT];

void drawGame(void);
void drawEdit(void);
void setOffsets(void);

extern HiScore    hiscores[MAX_HISCORES];
extern State     *state, *startState;
extern SDL_Color  black, white;
extern SDL_bool   inGame, hasWon, hasLost, hasQuit;
extern Uint8      px, py;	/* player location */
extern KuriLevel  levels[MAX_LEVELS];
extern KuriLevel *level;
extern Uint32     frame;

/*
=============
editor.c
=============
*/
int readMap(KuriLevel *level);
int writeMap(KuriLevel *level);

/*
=============
logic.c
=============
*/
int  isInBounds(Uint8 x, Uint8 y);
void tryUp(void);
void tryDown(void);
void tryLeft(void);
void tryRight(void);
void doTrap(void);
void doBombs(void);
void eatBlocks(void);
void slide(void);
void setField(void);
void resetField(void);
int  isStageClear(void);
void addHiScore(char *name, int score, int level);

/*
=============
menu.c
=============
*/
void doMenu(void);
void doGameWon(void);
void doGameLost(void);

#endif /* _KURI2D_H_ */

