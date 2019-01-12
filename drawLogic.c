#include "gameLogic.h"
#include "drawLogic.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <sys/time.h>

/* Unexported function definitions */
static void renderGame();
static void renderBackground();
static void renderScore();
static void renderTetris();
static void renderInScoreBox(char* buffer);
static void renderPlayingField();
static void renderCurrentTetromino();
static void renderHeldTetromino();
static void renderNextTetromino();
static void renderTetromino(int xCenter, int yCenter,int tetrominoShape, unsigned int tetrominoColor);
static void renderTetrominoShadow();
static void setRenderDrawColor(unsigned int rgb);
static void clearWindow();
static void updateDisplay();
static void handleKeyEvent(SDL_Event event);
static void updateDisplay();

/* Display constants */
const unsigned int tetrominoColors[8] = {0x000000,0x00FFFF,0xFFFF00,0x800080,0x008000,0xFF0000,0x0000FF,0xFFA500};

/* dependant on background texture */
#define PLAY_FIELD_X_OFFSET 150
#define PLAY_FIELD_Y_OFFSET 25

/* placeholder values */
#define WINDOW_HEIGHT 600 
#define WINDOW_WIDTH 400

/* dependant on background texture */
#define PLAY_FIELD_WIDTH (WINDOW_WIDTH - PLAY_FIELD_X_OFFSET - 25)
#define PLAY_FIELD_HEIGHT (WINDOW_HEIGHT - PLAY_FIELD_Y_OFFSET - 50)

#define BLOCK_WIDTH  (PLAY_FIELD_WIDTH / GAME_WIDTH)
#define BLOCK_HEIGHT (PLAY_FIELD_HEIGHT / GAME_HEIGHT)

#define REFRESH_RATE_MICROS 200000u
/* placeholder value */
#define INPUT_RATE_MICROS 5000u

#define SHADOW_COLOR 0x545454u

/*SDL vars*/
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* background;
TTF_Font *font;

void renderGame(){
    renderBackground();
    if(tetris){ 
        renderTetris();
    } else {
        renderScore();
    }
    renderPlayingField();
    renderTetrominoShadow();
    renderCurrentTetromino();

    renderHeldTetromino();
    renderNextTetromino();
}

void renderBackground(){
    SDL_RenderCopy(renderer,background,NULL,NULL);
}

void renderScore(){
    char buffer [12];
    sprintf(buffer,"Score %4d",score);
    renderInScoreBox(buffer);
}

void renderTetris(){
    char* buffer = "Tetris!";
    renderInScoreBox(buffer);
}

void renderInScoreBox(char* buffer){
    SDL_Color color = {0,0,0,0};
    SDL_Rect textRect = {PLAY_FIELD_X_OFFSET,WINDOW_HEIGHT-48,PLAY_FIELD_WIDTH,24};

    SDL_Surface* textSurface = TTF_RenderText_Solid(font,buffer,color);
    SDL_Texture* text = SDL_CreateTextureFromSurface(renderer,textSurface);
    SDL_RenderCopy(renderer,text,NULL,&textRect);
    SDL_DestroyTexture(text);
    SDL_FreeSurface(textSurface);
}

void renderPlayingField(){
    SDL_Rect gameSquare = {PLAY_FIELD_X_OFFSET,PLAY_FIELD_Y_OFFSET,BLOCK_WIDTH,BLOCK_HEIGHT};
    int x,y;
    for(y = 0; y < GAME_HEIGHT;y++){
        for(x = 0; x < GAME_WIDTH;x++){
            setRenderDrawColor(tetrominoColors[getPlayingField(x,y)]);
            SDL_RenderFillRect(renderer,&gameSquare);
            gameSquare.x += BLOCK_WIDTH;
        }
        gameSquare.y += BLOCK_HEIGHT;

        gameSquare.x = PLAY_FIELD_X_OFFSET;
    }
}

void renderCurrentTetromino(){
    int tetrominoShape = currentTetrominoShape();
    unsigned int tetrominoColor = tetrominoColors[currentTetromino+1];
    renderTetromino(currentTetrominoX, currentTetrominoY, tetrominoShape, tetrominoColor);
}

void renderTetrominoShadow(){
    int tetrominoShape = currentTetrominoShape();
    renderTetromino(shadowX(), shadowY(), tetrominoShape, SHADOW_COLOR);
}

void renderHeldTetromino(){
    if(heldTetromino >= 0){
        int tetrominoShape = tetrominos[heldTetromino];
        unsigned int tetrominoColor = tetrominoColors[heldTetromino+1];
        renderTetromino(-5,2,tetrominoShape,tetrominoColor);
    }
}

void renderNextTetromino(){
    int tetrominoShape = tetrominos[nextTetromino()];
    unsigned int tetrominoColor = tetrominoColors[nextTetromino()+1];
    renderTetromino(-5,10,tetrominoShape,tetrominoColor);
}

void renderTetromino(int xCenter, int yCenter,int tetrominoShape, unsigned int tetrominoColor ){
    setRenderDrawColor(tetrominoColor);
    int x,y;
    for(y=-2;y<2;y++){
        for(x=-2;x<2;x++){
            if(isSet(tetrominoShape,x+2,y+2)){
                SDL_Rect gameSquare = {
                    (xCenter + x)*BLOCK_WIDTH + PLAY_FIELD_X_OFFSET,
                    (yCenter + y)*BLOCK_HEIGHT + PLAY_FIELD_Y_OFFSET,
                    BLOCK_WIDTH,
                    BLOCK_HEIGHT
                };
                SDL_RenderFillRect(renderer,&gameSquare);
            }
        }
    }
}

void setRenderDrawColor(unsigned int rgb){
    SDL_SetRenderDrawColor(renderer,(rgb>>16)&0xFF,(rgb>>8)&0xFF,rgb&0xFF,255);
}

void clearWindow(){
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
}

void updateDisplay(){
    SDL_RenderPresent(renderer);
}

void runGame(){
    SDL_Event event;
    struct timeval tickStart;
    gettimeofday(&tickStart,NULL);
    while(1){
        clearWindow();
        renderGame();
        updateDisplay();

        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT){
            break;
        }


        struct timeval currentTime;
        gettimeofday(&currentTime,NULL);

        unsigned int elapsedMicroseconds = currentTime.tv_usec - tickStart.tv_usec;

        if(elapsedMicroseconds >= INPUT_RATE_MICROS){
            if(event.type == SDL_KEYDOWN && event.key.type == SDL_KEYDOWN){
                handleKeyEvent(event);
            }
        }

        if(elapsedMicroseconds >= REFRESH_RATE_MICROS - score*100){
            advanceGame();
            gettimeofday(&tickStart,NULL);
        }
    }
}

void handleKeyEvent(SDL_Event event){
    if(event.key.keysym.sym == SDLK_s){
        rotateTetrominoClockwise();
    }
    if(event.key.keysym.sym == SDLK_w){
        rotateTetrominoCounterClockwise();
    }
    if(event.key.keysym.sym == SDLK_a){
        moveTetromino(-1);
    }
    if(event.key.keysym.sym == SDLK_d){
        moveTetromino(1);
    }
    if(event.key.keysym.sym == SDLK_LSHIFT){
        holdTetromino();
    }
    if(event.key.keysym.sym == SDLK_SPACE){
        dropTetromino();
    }
}

void initSDL(){
    /*
     * Initialize SDL
     */
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Unable to initialize SDL:  %s\n", SDL_GetError());
    }

    atexit(SDL_Quit);

    if(TTF_Init()==-1)
    {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
        exit(0);
    }
    atexit(TTF_Quit);  //remember to quit SDL_ttf 
   // // atexit(free_font); /* remember to free any loaded font and glyph cache */

    font = TTF_OpenFont("LiberationMono-Regular.ttf", 24);
    if(font == NULL){
         fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
         exit(0);
    }

    /*
     * Initialize Window and renderer
     */
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH,WINDOW_HEIGHT,0,&window,&renderer);

    if (window == NULL) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
    }

    //FILE* backgroundFp = &_binary_background_bmp_start;
    FILE* backgroundFp = fopen("background.bmp","r");
    SDL_RWops* backgroundStream = SDL_RWFromFP(backgroundFp, SDL_TRUE);
    background = SDL_CreateTextureFromSurface(renderer,SDL_LoadBMP_RW(backgroundStream,1));

}

void destroySDL(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(background);
}
