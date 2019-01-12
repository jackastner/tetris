#include "drawLogic.h"

#ifdef __ANDROID__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

int main(int argc, char **argv){
    initSDL();
    runGame();
    destroySDL();
}
