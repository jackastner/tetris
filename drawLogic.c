#include "gameLogic.h"
#include "drawLogic.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <sys/time.h>

/* Unexported function definitions */
static void renderGame();
static void renderMenu();
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
static int handleKeyEvent(SDL_Event event);
static void handleGameEvent(SDL_Event event);
static int handleMenuEvent(SDL_Event event);
static int handleButtonClick(int buttonId);
static void updateDisplay();
static void renderButton(SDL_Rect* button_rect, char* text);

/* Display constants */
const unsigned int tetrominoColors[8] = {0x202020,0x00FFFF,0xFFFF00,0x800080,0x008000,0xFF0000,0x0000FF,0xFFA500};

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

#define SHADOW_COLOR 0x545454u

#define BACKGROUND_COLOR 0x000000

/*SDL vars*/
SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font *font;

typedef enum {
    BUTTON_PLAY,
    BUTTON_QUIT,
    BUTTON_COUNT
} Button;

char* buttonStrings[] = {
    "Play",
    "Quit"
};

SDL_Rect buttonRects[] = {
    {
        WINDOW_WIDTH / 12,
        WINDOW_HEIGHT / 2,
        WINDOW_WIDTH / 4,
        WINDOW_HEIGHT / 10
    },
    {
        WINDOW_WIDTH / 12,
        13 * WINDOW_HEIGHT / 20,
        WINDOW_WIDTH / 4,
        WINDOW_HEIGHT / 10
    }
};


void renderGame(){
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

void renderMenu(){
    renderButton(&buttonRects[BUTTON_PLAY], buttonStrings[BUTTON_PLAY]);
    renderButton(&buttonRects[BUTTON_QUIT], buttonStrings[BUTTON_QUIT]);
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
    SDL_Color color = {0xff,0xff,0,0};
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

void renderButton(SDL_Rect* button_rect, char* text){
    setRenderDrawColor(0xffff00);
    SDL_RenderDrawRect(renderer, button_rect);

    SDL_Color color = {0xff,0xff,0x00};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer,textSurface);
    SDL_RenderCopy(renderer,textTexture,NULL,button_rect);
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

void setRenderDrawColor(unsigned int rgb){
    SDL_SetRenderDrawColor(renderer,(rgb>>16)&0xFF,(rgb>>8)&0xFF,rgb&0xFF,255);
}

void clearWindow(){
    setRenderDrawColor(0x200020);
    SDL_RenderClear(renderer);
}

void updateDisplay(){
    SDL_RenderPresent(renderer);
}

void runGame(){
    SDL_Event event;

    Uint32 tick_interval = 100;
    Uint32 poll_interval = 50;

    Uint32 last_tick = 0;
    Uint32 last_poll = 0;

    resetState();

    while(1){
        clearWindow();
        renderGame();
        if(isGameOver()){
            renderMenu();
        }
        updateDisplay();

        SDL_PumpEvents();
        if(SDL_PeepEvents(&event,1,SDL_PEEKEVENT,SDL_QUIT,SDL_QUIT)){
            break;
        }

        Uint32 currentTime = SDL_GetTicks();

        if(SDL_TICKS_PASSED(currentTime, last_poll + poll_interval)){
            while(SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT)){
               if(!handleKeyEvent(event)){
                   break;
               }
            }
            last_poll = currentTime;
        }

        if(!isGameOver() && SDL_TICKS_PASSED(currentTime, last_tick + tick_interval)){
            advanceGame();
            last_tick = currentTime;
        }
    }
}

int handleKeyEvent(SDL_Event event){
    if(isGameOver()){
        return handleMenuEvent(event);
    } else {
        handleGameEvent(event);
    }
    return 1;
}


int handleButtonClick(int buttonId){
    switch (buttonId){
        case BUTTON_PLAY:
            resetState();
            break;
        case BUTTON_QUIT:
            SDL_Quit();
            return 0;
    }
    return 1;
}


int handleMenuEvent(SDL_Event event){
    if(event.type == SDL_MOUSEBUTTONDOWN){
        SDL_Point p = {event.button.x, event.button.y};
        for(int i = 0; i < BUTTON_COUNT; i++){
            SDL_Rect buttonRect = buttonRects[i];
            if(SDL_PointInRect(&p, &buttonRect)){
                return handleButtonClick(i);
            }
        }
    }
    return 1;
}

void handleGameEvent(SDL_Event event){
    if(event.type == SDL_KEYDOWN && event.key.type == SDL_KEYDOWN){
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
}

void destroySDL(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
}
