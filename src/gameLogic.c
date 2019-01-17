#include "gameLogic.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

const int tetrominos[7] = {0x0F00, 0x0660, 0x0E20, 0x0740, 0x0360, 0x0630, 0x0720};

int playingField[GAME_HEIGHT][GAME_WIDTH];

/* public state variables */
int score;
int tetris;
int prevTetris;
int heldTetromino;
int currentTetromino;
int nextTetrominoIdx;
int currentTetrominoX;
int currentTetrominoY;
static int rotationState;
static int holdUsed;

/* private functions */
static int atBottom();
static int atTop();
static int tetrominoTop(int tetromino);
static int tetrominoBottom(int tetromino);
static int tetrominoLeft(int tetromino);
static int tetrominoRight(int tetromino);
static int pastRightBound();
static int pastLeftBound();
static void clearRows(int start, int end);
static void tryClearRows();
static void setTetromino(int tetrominoNum);
static void startNextTetromino();

int isGameOver(){
    return atBottom() && atTop();
}

void resetState(){
    srand(time(NULL));
    score = 0;
    tetris = 0;
    prevTetris = 0;
    heldTetromino = -1;
    rotationState = 0;
    holdUsed = 0;
    bzero(playingField, sizeof(int) * GAME_HEIGHT * GAME_WIDTH);
    startNextTetromino();
}

void placeTetromino() {
    for(int y=-2; y<2 ;y++){
        for(int x=-2; x<2; x++){
            if(isSet(tetrominos[currentTetromino], x+2, y+2)){
                if(currentTetrominoY + y < 0 || 
                   currentTetrominoY + y >= GAME_HEIGHT ||
                   currentTetrominoX + x < 0 ||
                   currentTetrominoX + x >= GAME_WIDTH){
                    printf("out of bounds %d,%d\n", currentTetrominoY + y, currentTetrominoX + x);
                    break;
                }
                playingField[currentTetrominoY + y][currentTetrominoX + x] = currentTetromino + 1;
            }
         }
    }
}

void dropTetromino() {
    while(!atBottom()){
        currentTetrominoY++;
    }
}

void holdTetromino() {
    if(!holdUsed){
        int temp = heldTetromino;
        heldTetromino = currentTetromino;
        if(temp >= 0){
            setTetromino(temp);
        } else {
            startNextTetromino();
        }
        currentTetrominoY++;
        holdUsed = 1;
    }
}

void moveTetromino(int direction) {
    currentTetrominoX += direction;

    currentTetrominoY--;
    if(atBottom()){
        currentTetrominoX -= direction;
    } else if(pastLeftBound()){
        currentTetrominoX = -tetrominoLeft(tetrominos[currentTetromino]);
    } else if (pastRightBound()) {
        currentTetrominoX = GAME_WIDTH - 1 - tetrominoRight(tetrominos[currentTetromino]);
    }
    currentTetrominoY++;
}

int pastLeftBound() {
    return currentTetrominoX + tetrominoLeft(tetrominos[currentTetromino]) < 0;
}

int pastRightBound() {
    return currentTetrominoX >= GAME_WIDTH - tetrominoRight(tetrominos[currentTetromino]);
}

void rotateTetrominoCounterClockwise(){
    int temp = rotationState;
    rotationState = (rotationState + 3) % 4;
    if(atBottom() || pastLeftBound() || pastRightBound()){
        rotationState = temp;
    }
}

void rotateTetrominoClockwise() {
    int temp = rotationState;
    rotationState = (rotationState + 1) % 4;
    if(atBottom() || pastLeftBound() || pastRightBound()){
        rotationState = temp;
    }
}

void advanceGame() {
    tetris = 0;
    if(atBottom()){
        placeTetromino();
        tryClearRows();
        startNextTetromino();
        holdUsed = 0;
    }
    currentTetrominoY++;
}

void startNextTetromino(){
    setTetromino(nextTetrominoIdx);
    nextTetrominoIdx = rand() % 7;
}

int nextTetromino(){
    return nextTetrominoIdx;
}

void setTetromino(int tetrominoNum){
    currentTetromino = tetrominoNum;
    currentTetrominoX = GAME_WIDTH / 2;
    currentTetrominoY =  -(tetrominoTop(tetrominos[currentTetromino]) + 1);
}

void tryClearRows() {
    int first_full = 0;
    int in_run = 0;

    for(int r = 0; r < GAME_HEIGHT; r++){
        int row_full = 1;
        for(int c = 0; row_full && c < GAME_WIDTH; c++){
            if(!playingField[r][c]){
                row_full= 0;
                break;
            }
        }

        if(row_full && !in_run){
            first_full = r;
            in_run = 1;
        }

        if(!row_full && in_run){
            clearRows(first_full, r);
            in_run = 0;
        }
    }

    if(in_run){
        clearRows(first_full, GAME_HEIGHT);
    }
}

void clearRows(int start, int end){
    int clear_size = end-start;
    if(clear_size == 4){
        if(prevTetris){
            score += 1200;
        } else {
            score += 800;
        }
        tetris = 1;
        prevTetris = 1;
    } else {
        prevTetris = 0;
        score += clear_size * 100;
    }
    for(int r = end - 1; r >= clear_size; r--){
        for(int c = 0; c < GAME_WIDTH; c ++){
            playingField[r][c] = playingField[r - clear_size][c];
        }
    }

    for(int r = 0; r < clear_size; r++){
        for(int c = 0; c < GAME_WIDTH; c ++){
            playingField[r][c] = 0;
        }
    }
}

int tetrominoBottom(int tetromino){
    for(int y=1; y>=-2 ;y--){
        for(int x=-2; x<2; x++){
            if(isSet(tetromino, x+2, y+2)){
                return y;
            }
        }
    }

    return 0;
}

int tetrominoTop(int tetromino){
   for(int y=-2; y<2 ;y++){
        for(int x=-2; x<2; x++){
            if(isSet(tetromino, x+2, y+2)){
                return y;
            }
        }
    }

    return 0;
}

int tetrominoLeft(int tetromino){
   for(int x=-2; x<2; x++){
     for(int y=-2; y<2; y++){
        if(isSet(tetromino, x+2, y+2)){
            return x;
        }
     }
   }

   return 0;
}

int tetrominoRight(int tetromino){
   for(int x=1; x>=-2; x--){
     for(int y=-2; y<2; y++){
        if(isSet(tetromino, x+2, y+2)){
            return x;
        }
     }
   }

   return 0;
}

int atBottom(){
    for(int y=-2; y<2 ;y++){
        for(int x=-2; x<2; x++){
            if(isSet(tetrominos[currentTetromino], x+2, y+2) && (currentTetrominoY + y + 1 >= GAME_HEIGHT || getPlayingField(currentTetrominoX + x, currentTetrominoY + y + 1))){
                return 1;
            }
        }
    }
    return 0;
}

int atTop() {
    return currentTetrominoY ==  -tetrominoTop(tetrominos[currentTetromino]);
}

int isSet(int tetrominoShape, int x, int y) {
    int rot_x, rot_y;
    if(rotationState == 0 || rotationState == 2){
        rot_x = x;
        rot_y = y;
    } else {
        rot_x = y;
        rot_y = x;
    }

    if(rotationState == 2 || rotationState == 3 || rotationState == 4){
        rot_x = 3 - rot_x;
    }

    if(rotationState == 1 || rotationState == 2){
        rot_y = 3 - rot_y;
    }

    return (tetrominoShape >> (4*rot_y + rot_x)) & 1;
}

int currentTetrominoShape() {
    return tetrominos[currentTetromino];
}

int shadowX() {
    return currentTetrominoX;
}

int shadowY() {
    int temp = currentTetrominoY;
    dropTetromino();
    int ret = currentTetrominoY;
    currentTetrominoY = temp;

    return ret;
}

int getPlayingField(int x, int y) {
    return playingField[y][x];
}
