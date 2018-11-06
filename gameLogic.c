#include "gameLogic.h"

const int tetrominos[5] = {0x0F00, 0x0AA0, 0x08A2, 0x0B20};
const int GAME_HEIGHT = 30;
const int GAME_WIDTH = 12;

int score = 0;
int tetris = 0;
int heldTetromino = 0;
int currentTetromino = 0;
int currentTetrominoX = 0;
int currentTetrominoY = 0;

void dropTetromino() {

}

void holdTetromino() {

}

void moveTetromino(int direction){

}

void rotateTetrominoCounterClockwise(){

}
void rotateTetrominoClockwise() {

}

void advanceGame() {

}

int isSet(int tetrominoShape, int x, int y) {
    return (tetrominoShape >> (4*y + x)) & 1;
}

int currentTetrominoShape() {
    return currentTetromino;
}

int shadowX() {
    return 0;
}

int shadowY() {
    return 0;
}

int getPlayingField(int x, int y) {
    return 0;
}
