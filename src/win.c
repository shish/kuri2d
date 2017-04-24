#include "kuri2d.h"

/**
 *
 */
void initWin() {
    resetField();
}

void doWin() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {hasQuit = K2D_TRUE;}
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE: hasQuit = K2D_TRUE; break;
                default:
                    setScreen(SCR_GAME);
                    break;
            }
        }
    }

    drawTexture(backgrounds[BG_BACK], 0, 0);
    drawTexture(backgrounds[BG_WIN], 32, 192);
}
