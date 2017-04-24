#include "kuri2d.h"

/**
 *
 */
char name[NAME_LEN] = "";
int namePos = 0, hiPos = 0;

void initLose() {
    name[0] = '\0';
    namePos = 0, hiPos = 0;

    resetField();
}

void doLose() {
    /* check for hiscore */
    for(hiPos=0; hiPos<MAX_HISCORES; hiPos++) {
        if(state->score > hiscores[hiPos].score) {
            break;
        }
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {hasQuit = K2D_TRUE;}
        if(event.type == SDL_KEYDOWN) {
            if(hiPos < MAX_HISCORES) {
                if(event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z) {
                    if(namePos < NAME_LEN) {
                        name[namePos++] = event.key.keysym.sym;
                        name[namePos] = 0;
                    }
                }
                else {
                    switch (event.key.keysym.sym) {
                        case SDLK_RETURN:
                            addHiScore(name, state->score, state->level);
                            setScreen(SCR_MENU);
                            break;
                        case SDLK_BACKSPACE:
                            if(namePos > 0) name[--namePos] = 0;
                            break;
                        default:
                            break;
                    }
                }
            }
            else if(event.type == SDL_KEYDOWN) {
                setScreen(SCR_MENU);
            }
        }
    }

    drawTexture(backgrounds[BG_BACK], 0, 0);
    drawTexture(backgrounds[BG_LOSE], 32, 192);

    if(hiPos < MAX_HISCORES) {
        char hiText[64];
        snprintf(hiText, 64, "New Hiscore: %i points!", state->score);
        drawString(fonts[FONT_SCORE], hiText, black, 100, 300);
        snprintf(hiText, 64, "Enter Name: %s", name);
        drawString(fonts[FONT_SCORE], hiText, black, 100, 332);
    }
}


/**
 * Figure out if the player gets a place in the charts, if they do, add them
 */
void addHiScore(char *name, int score, int level) {
    int i, j;

    for(i=0; i<MAX_HISCORES; i++) {
        if(score >= hiscores[i].score) {
            printf("Player got a hi-score! (%s, %i, %i) Inserting into position %i\n", name, score, level, i);
            for(j=MAX_HISCORES-1; j>i; j--) memcpy(&hiscores[j], &hiscores[j-1], sizeof(HiScore));
            strncpy(hiscores[i].name, name, 8);
            hiscores[i].score = score;
            hiscores[i].level = level;
            break;
        }
    }
}
