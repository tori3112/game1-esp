//
// Created by wiktoria on 28/03/24.
//

#ifndef GAME_ALPHABETA_H
#define GAME_ALPHABETA_H


#include "board.h"
#include "game.h"

typedef struct {
    uint64_t position;
    uint64_t mask;
    int no_moves;
} bitboard;

/**
 * at the beginning of the game:
 * position is zero
 * mask is zero
 * bottom has all ones at the bottom
 */
void initialise_bitboard(bitboard* bb);

/**
 * check if a column is playable
 * check if the last cell of the column is free by
 * by checking the bottom board
 */
bool can_play(uint64_t bb_mask, int col);

/**
 * make move into column by
 * 1) switch bits of current player and opponent
 * 2) add extra bit to mask (into specific column)
 */
void play(bitboard *bb, int col);

/**
 * method checks whether there is a four in a row
 * for the current player
 */
bool check_win(uint64_t position);

/**
 * checks whether the move into @param col
 * leads to immediate win
 */
bool is_win(bitboard bb, int col);

/**
 * negamax algorithm with alpha-beta pruning implementation
 * with ints (not bits)
 */
move negamax_ab(int board[ROWS*COLS], int alpha, int beta, int depth, int turn);

/**
 * spits out order in which the columns are explored
 * from centre to the edges
 * @param width is the number of columns
 * @return
 */
int* get_exploration_order(int width);

/**
 * negamax algorithm with alpha-beta pruning
 * with bitboards implementation
 */
move negamax_ab_bb(const bitboard bb, int alpha, int beta, int depth);

/**
 * method to generate a score for a specific position board
 * higher score is generated when board has 3 or 2 in a line
 */
int evaluate_bb(const uint64_t position);

// HELPER METHODS
/**
 * identifies the lowest available slot
 * in a specific column
 * of a bitwise representation of a game board
 */
uint64_t bottom_cell(int col);

/**
 * identifies the top cell
 * in a specific column
 * of a bitwise representation of a game board
 */
uint64_t top_cell(int col);

/**
 * selects a specific column from a board
 * in a nutshell, a bitboard with 1s only in column specified by @param col
 */
uint64_t get_column(int col);

void print_bitboard(bitboard bb);

#endif //GAME_ALPHABETA_H