//
// Created by wiktoria on 15/12/23.
//

#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <string.h>
#include <stdbool.h>
#include "board.h"

#define ROWS 6
#define COLS 7

// specify scores for evaluation
#define WIN_SCORE 110
#define HIGH_SCORE 50
#define MEDIUM_SCORE 20

typedef struct {
    int board[ROWS*COLS]; //using modulo -- "field_idx = row * COLS + col"
    char position_notation[ROWS*COLS];
} board;

/**
 * this method considers some possible game trajectories
 * @return value of the board
 */

void assignBoard(int board[ROWS*COLS]);

void assignPositionString(char position_string[ROWS*COLS]);


#endif // GAME_GAME_H
