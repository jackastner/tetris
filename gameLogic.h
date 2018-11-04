#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H
const int tetrominos[5];
const int GAME_HEIGHT;
const int GAME_WIDTH;

/* public game state variables */
int score;
int tetris;
int heldTetromino;
int currentTetromino;
int currentTetrominoX;
int currentTetrominoY;

/* public game mutator functions */
void dropTetromino();
void holdTetromino();
void moveTetromino(int direction);
void rotateTetrominoCounterClockwise();
void rotateTetrominoClockwise();
void advanceGame();

/* public game accessor functions */
int isSet(int tetrominoShape, int x, int y);
int currentTetrominoShape();
int shadowX();
int shadowY();
int getPlayingField(int x, int y);

#endif
