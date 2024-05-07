//
// Created by wiktoria on 28/03/24.
//

/**
 * THE (BIT) BOARD
 * 41 40 39 38 37 36 35
 * 34 33 32 31 30 29 28
 * 27 26 25 24 23 22 21
 * 20 19 18 17 16 15 14
 * 13 12 11 10 09 08 07
 * 06 05 04 03 02 01 00
 */

// include standard C libraries
#include <stdio.h>
#include <malloc.h>
#include <limits.h>
#include <time.h>
#include <stdlib.h>

// include project dependencies
#include "alphabeta.h"
#include "board.h"

void initialise_bitboard(bitboard* bb){
    bb->position=0;
    bb->mask=0;
    bb->no_moves=0;
}

bool can_play(uint64_t bb_mask, int col){
    if ((col < 0) | (col > COLS)) {
        return false;
    }
    return !(bb_mask & top_cell(col));
}

void play(bitboard *bb, int col){
    //printf("\nNO_MOVES: %d,\nMASK: %lu,\nPOSITION: %lu\n",bb->no_moves,bb->mask,bb->position);
    if (!can_play(bb->mask,col)) {
        return;
    } else {
        // STEP 1: MAKE A MOVE INTO COLUMN
        if ((bb->mask & get_column(col)) == 0) {
            bb->mask |= bottom_cell(col);
        } else {
            bb->mask |= ((bb->mask & get_column(col)) << COLS);
        }
        // STEP 2: FLIP POSITION TO CURRENT (NEXT?) PLAYER
        bb->position ^= bb->mask;
        // STEP 3: INCREASE NO OF MOVES
        bb->no_moves++;
    }
}

bool check_win(uint64_t position) {
    // VERTICAL CHECK
    uint64_t check = position & (position << COLS);
    if (check & (check << 2*COLS)) {
        return true;
    }
    // HORIZONTAL CHECK
    check = position & (position << 1);
    if (check & (check << 2)) {
        return true;
    }
    // POSITIVE DIAGONAL
    check = position & (position << (COLS-1));
    if (check & (check << 2*(COLS-1))) {
        return true;
    }
    // NEGATIVE DIAGONAL
    check = position & (position << (COLS+1));
    if (check & (check << 2*(COLS+1))) {
        return true;
    }
    return false;

}

bool is_win(bitboard bb, int col) {
    uint64_t move_col = (bb.mask & get_column(col));
    uint64_t move = (move_col ^ (move_col << COLS)) ^ bottom_cell(col);
    uint64_t position = bb.position | move;
    return check_win(position);
}

int evaluate_bb(const uint64_t position) {
    int score = 0;
    //VERTICAL EVALUATION
    uint64_t check = position & (position << COLS);
    if (check & (check << COLS)) {
        score += HIGH_SCORE;
    } else if (check) {
        score += MEDIUM_SCORE;
    }
    //HORIZONTAL EVALUATION
    check = position & (position << 1);
    if (check & (check << 1)) {
        score += HIGH_SCORE;
    }
    else if (check) {
        score += MEDIUM_SCORE;
    }
    //POSITIVE DIAGONAL EVALUATION
    check = position & (position << (COLS-1));
    if (check & (check << (COLS-1))) {
        score += HIGH_SCORE;
    }
    else if (check) {
        score += MEDIUM_SCORE;
    }
    //NEGATIVE DIAGONAL EVALUATION
    check = position & (position << (COLS+1));
    if (check & (check << (COLS+1))) {
        score += HIGH_SCORE;
    }
    else if (check) {
        score += MEDIUM_SCORE;
    }
    return score;
}

uint64_t bottom_cell(int col) {
    return 1ULL << col;
}

uint64_t top_cell(int col) {
    return 1ULL << ((ROWS-1)*COLS+col);
}

uint64_t get_column(int col) {
    uint64_t out = 0;
    uint64_t column = 1ULL << col;
    for (int i=0; i<ROWS; i++) {
        out += column << (i*COLS);
    }
    return out;
}

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
            add_coin(copy,i,turn);
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

move negamax_ab_bb(const bitboard bb, int alpha, int beta, int depth) {
    if (bb.no_moves == ROWS*COLS-2) {
        move next_move = {-1,0};
        return next_move;
    }
    if (depth == 0) {
        move dummy = {6, evaluate_bb(bb.position)};
        return dummy;
    }
    for (int i=0; i<COLS; i++) {
        if (can_play(bb.mask,i) && is_win(bb,i) ) {
            move move1 = {i,WIN_SCORE};
            //printBoard(board);
            return move1;
        }
    }
    move best_move = {-1, INT_MIN};
    int *order = get_exploration_order(COLS);
    for (int i=0; i<COLS; i++) {
        if (can_play(bb.mask,order[i])) {
            bitboard copy = {bb.position,bb.mask,bb.no_moves};
            //printf("or: %lu vs cp: %lu\n",bb.position,copy.position);
            play(&copy,order[i]);
            move next_move = negamax_ab_bb(copy,-beta, -alpha, depth-1);
            next_move.score = -next_move.score;
            if (best_move.score <= alpha) {
                best_move.score = next_move.score;
                best_move.col = next_move.col;
            }
            if (alpha > beta) {
                break;
            }
        }
    }
    return best_move;
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