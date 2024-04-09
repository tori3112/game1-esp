//
// Created by wiktoria on 28/03/24.
//

// include standard C libraries
#include <stdio.h>
#include <malloc.h>
#include <limits.h>
#include <time.h>
#include <stdlib.h>

// include project dependencies
#include "alphabeta.h"
#include "board.h"

move negamax_ab(int board[ROWS*COLS], int alpha, int beta, int depth, int turn) {
    if (getNoMoves(board) == 40) {
        move next_move = {-1,0};
        return next_move;
    }
    if (depth == 0) {
        srand(time(NULL));
        int rand_col = rand()%COLS;
        move dummy = {rand_col, evaluate(board)};
        return dummy;
    }
    for (int i=0; i<COLS; i++) {
        if (can_add_coin(board,i) && is_it_win_move(board,i,turn) ) {
            move move1 = {i,WIN_SCORE};
            //printBoard(board);
            return move1;
        }
    }
    move test_move = {-1,INT_MIN};
    int *order = get_exploration_order(COLS);
    int *copy = malloc(ROWS*COLS* sizeof(int));
    for (int i=0; i<COLS; i++) {
        if (can_add_coin(board,order[i])) {
            copy_board(board,copy);
            add_coin(copy,order[i],turn);
            move next_move = negamax_ab(board,-beta, -alpha, depth-1,5-turn);
            next_move.score = -next_move.score;
            if (test_move.score <= alpha) {
                test_move.score = next_move.score;
                test_move.col = next_move.col;
            }
            if (alpha > beta) {
                break;
            }
        }
    }
    free(copy);
    return test_move;
}

int *get_exploration_order(int width) {
    int *order = malloc(width* sizeof(int));
    if (order == NULL) {
        return NULL; //handling error in memory allocation
    }

    for (int i=0; i<width; i++) {
        order[i] = width/2 + (1-2*(i%2))*(i+1)/2;
    }

    return order;
}
