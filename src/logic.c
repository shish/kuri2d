/**
 * vim:ts=4:sw=4
 * logic.c - (c) Shish 2003, 2005 - shish@shish.is-a-geek.net
 * an extension of main.c to handle the logic parts
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <stdlib.h>
#include "kuri2d.h"

static void trySet   (Uint8 x, Uint8 y);
static void detonate (Uint8 x, Uint8 y);
static k2d_boolean eatBlock (Uint8 x, Uint8 y);

/** (private for tryXXX)
 * to see if the player is allowed to move there
 */
static int isInBounds(Uint8 x, Uint8 y) {
	return (x < level->fieldWidth && y < level->fieldHeight);
}
static int isFree(Uint8 x, Uint8 y) {
	return (isInBounds(x, y) &&
			(level->field[x][y] == BT_EMPTY ||
			level->field[x][y] == BT_ABOMB ||
			level->field[x][y] == BT_TRAP ||
			level->field[x][y] == BT_TRIGGERED) &&
			level->blocks[x][y] == BT_NULL);
}
static void tryMove(int dx, int dy) {
    if(isInBounds(state->px, state->py) && isFree(state->px + dx, state->py + dy)) {
        state->px += dx;
        state->py += dy;
    }
}

void tryUp()    {tryMove(0, -1);}
void tryDown()  {tryMove(0, 1);}
void tryLeft()  {tryMove(-1, 0);}
void tryRight() {tryMove(1, 0);}


/**
 * scan for traps - if there are any, trigger them.
 * if no traps are found, then set one at the player's current location
 */
void doTrap() {
	Uint8 x, y;

	for (y = 0; y < level->fieldHeight; y++) {
		for (x = 0; x < level->fieldWidth; x++) {
			if (level->field[x][y] == BT_TRAP) {
				level->field[x][y] = BT_TRIGGERED;
				return;
			}
		}
	}

	level->field[state->px][state->py] = BT_TRAP;
}


/** (private to detonate)
 * if possible, sets a square to triggered
 * used to stop things like field[-1][5] = BT_TRIGGERED;
 * checking for BT_BOMB stops bombs overwriting eachother
 * the same for BT_TRAP
 */
static void trySet(Uint8 x, Uint8 y) {
	if(x < level->fieldWidth && y < level->fieldHeight) {
		if(level->field[x][y] != BT_ABOMB && level->field[x][y] != BT_TRAP)
			level->field[x][y] = BT_TRIGGERED;
	}
}


/** (private to doBombs)
 * sets all the squares next to a bomb to triggered
 * WARNING: beware of wraparound! The board is only 256 blocks across,
 * so triggering -1,-1 will affect 256,256
 */
static void detonate(Uint8 x, Uint8 y) {
	trySet(x-1, y-1); trySet(x  , y-1);                  trySet(x+1, y-1);
	trySet(x-1, y  ); level->field[x][y] = BT_TRIGGERED; trySet(x+1, y  );
	trySet(x-1, y+1); trySet(x  , y+1);                  trySet(x+1, y+1);
}

     
/**
 * sets off all the bombs in the field
 */
void doBombs() {
	Uint8 x, y;

	for(y=0; y<level->fieldHeight; y++) {
		for(x=0; x<level->fieldWidth; x++) {
			if(level->field[x][y] == BT_ABOMB)
				detonate(x, y);
		}
	}

	/* don't let blocks get eaten mid-step */
	/* this doesn't work very well */
	/* if(frame % 32 < 2 || frame % 32 > 28) eatBlocks(); */
}


/** (private to slide)
 * (You know you've been programming too long when you turn your code
 * comments into ASCII space tanks...)
 *
 *    \\
 *     ||
 *     ||
 *   oooooooooooooooooo
 * move the specified block down a notch                   oo
 * 1) if it moves on top of field[x][y] == BT_TRIGGERED, then:
 *   o) remove it   ooo
 *   o) do the block thing:
 *      a) regular - add points
 *      b) advantage - set bomb
 *      c) forbidden - remove last row
 *   o) play the "drop" sound        oooooooooooooooooo
 * 2) if it moves on top of field[x][y] == BT_PLAYER, then kill the player
 * 3) if it moves off of the end of the stage, remove it and do the block thing
 *   a) regular - take off a sub-life                          oooooooooooooo
 *   b) advantage - do nothing                               oooooooooooooooooo
 *   c) forbidden - add points                                 oooooooooooooo
 *
 */
static void slideBlock(Uint8 x, Uint8 y) {
	if(y == level->fieldHeight) {	/* goes off the end */
		switch(level->blocks[x][y]) {
			case BT_BLOCK:		state->score -= 10; break;
			/* case BT_BOMB:	state->score -= 20; break; */
			case BT_FORBIDDEN:	state->score += 30; break;
			default: break;
		}
		return;
	}

	level->blocks[x][y + 1] = level->blocks[x][y];
	level->blocks[x][y] = BT_NULL;
	y++; /* set the current block as one block lower */
	if(level->blocks[x][y] != BT_NULL) {
		if(x == state->px && y == state->py) {
			/* got crushed, game over */
			setScreen(SCR_LOSE);
			playSound(CH_END, sounds[SND_DIE]);
		}
	}
}


/**
 * call slideBlock() for each block in blocks[][]
 * check if any blocks have slod (?) into traps
 * reset traps
 */
void slide() {
	Uint8 x, y;

	/* this is as good a place as any to put this -
	 * after any forbidden blocks are eaten, the field
	 * will shrink, this checks to see if the player
	 * is over the edge
	 */
	if(state->py >= level->fieldHeight) {
		kill();
	}

	/*
	 * slide bottom row first, then go up
	 * Stupid hack because of unsigned numbers - y will
	 * never reach -1 >:(
	 */
	for(y = level->fieldHeight; y > 0; ) {
		y--;
		for (x = 0; x < level->fieldWidth; x++) {
			slideBlock(x, y);
		}
	}

	eatBlocks();

	/* triggered blocks are temporary, clear them each slide() */
	for (y = 0; y < level->fieldHeight; y++) {
		for (x = 0; x < level->fieldWidth; x++) {
			if (level->field[x][y] == BT_TRIGGERED)
				level->field[x][y] = BT_EMPTY;
		}
	}
}


/**
 * go through the field looking for set traps,
 * eat anything that falls into them
 */
void eatBlocks() {
	Uint8 x, y;
	k2d_boolean set = K2D_FALSE;

	for (y = 0; y < level->fieldHeight; y++) {
		for (x = 0; x < level->fieldWidth; x++) {
			if(level->field[x][y] == BT_TRIGGERED) set |= eatBlock(x, y);
		}
	}
	if(set) playSound(CH_DROP, sounds[SND_DROP]);
}


/**
 * if a block is over a trap, eat the block
 */
static k2d_boolean eatBlock(Uint8 x, Uint8 y) {
	k2d_boolean set = K2D_TRUE;

	if (level->blocks[x][y] != BT_NULL) {
		if (level->field[x][y] == BT_TRIGGERED) {
			switch (level->blocks[x][y]) {
				case BT_BLOCK:
					state->score += 20;
					level->field[x][y] = BT_EMPTY;
					break;
				case BT_BOMB:
					state->score += 20;
					level->field[x][y] = BT_ABOMB;
					break;
				case BT_FORBIDDEN:
					state->score -= 60;
					if(level->fieldHeight > 5) level->fieldHeight--;
					level->field[x][y] = BT_EMPTY;
					break;
				default:
					set = K2D_FALSE;
					break;
			}
			level->blocks[x][y] = BT_NULL;
		}
		else {
			set = K2D_FALSE;
		}
	}

	return set;
}


/**
 * sets all the tiles to BT_EMPTY
 */
void resetField() {
	Uint8 x, y;

	for (y = 0; y < level->fieldHeight; y++) {
		for (x = 0; x < level->fieldWidth; x++) {
			level->field[x][y] = BT_EMPTY;
		}
	}
}


/**
 * checks to see if all the blocks[][] are gone
 */
int isStageClear() {
	Uint8 x, y;

	for (y = 0; y < level->fieldHeight; y++) {
		for (x = 0; x < level->fieldWidth; x++) {
			if(level->blocks[x][y] != BT_NULL) return 0;
		}
	}

	return 1;
}
