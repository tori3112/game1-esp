//
// Created by wiktoria on 15/12/23.
//

#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include <stdbool.h>
#include "game.h"

// specify board dimensions
#define ROWS 6
#define COLS 7
// specify game move details
#define HUMAN 2
#define COMPUTER 3
#define EMPTY 0

typedef struct {
    int col;
    int score;
} move;

/**
 * checks whether a specific column in a board is full
 * necessary to check available moves
 * @param board
 * @param col
 * @return true if the top field within the column is not empty
 */
bool is_column_full(const int board[ROWS*COLS], int col);

/**
 * check if there is a line of four
 * @param board because we are going to check on a specific board
 * @return true if we got a four in line and false if there is none
 * might use Bresenham's line algorithm
 * to be used in recursive method for the moves
 */
bool check_four(const int board[ROWS*COLS]);

/**
 * checks whether the next move is a win move
 * @param board
 * @param col
 * @return true if after the move into @param col game is over
 */
bool is_it_win_move(const int board[ROWS*COLS], int col, int turn);
bool check_vertical(const int board[ROWS*COLS], int col,int turn);
bool check_horizontal(const int board[ROWS*COLS], int col,int turn);
bool check_diagonal_positive(const int board[ROWS*COLS], int col,int turn);
bool check_diagonal_negative(const int board[ROWS*COLS], int col,int turn);
/**
 * checks whether the move is possible
 * not only if the column is full
 * but whether @param col is within valid range
 * @param board
 * @param col
 * @return true is the move is valid
 */
bool can_add_coin(const int board[ROWS*COLS], int col);

/**
 * for human turn, inputs coins into non-material board,
 * initially, based on human input number
 * later on, this will be updated to incorporate sensor input
 * @param board is the board we are updating
 * @param col_no is the column number in which we input coin
 * @param turn shows whose turn it is (human or computer)
 * @return should it be bool or int to make sure it didn't fail?
 */
void add_coin(int *board, int col, int turn);

/**
 * returns the row of the first 0 in the column
 * form the top
 * @param board
 * @param col
 * @return
 */
int filled_level(const int board[ROWS*COLS], int col);

/**
 * prints out the present board set up
 * @param board
 */
void printBoard(const int *board);

/**
 * @return number of moves made since the beginning of the game
 * based on non-zero fields on the board
 */
int getNoMoves(const int board[ROWS*COLS]);

/**
 * fill the board with zeros
 * @param board
 */
void initializeBoard(int *board);

/**
 *
 * @param board
 * @return
 */
int evaluate(const int board[ROWS*COLS]);
int evaluate_vertical(const int board[ROWS*COLS]);
int evaluate_horizontal(const int board[ROWS*COLS]);
int evaluate_positive_diagonal(const int board[ROWS*COLS]);
int evaluate_negative_diagonal(const int board[ROWS*COLS]);

move negamax(int board[ROWS*COLS], int depth, int turn);

void copy_board(const int board[ROWS*COLS], int copy[ROWS*COLS]);
int *get_exploration_order(int width);
bool test_copy(const int board[ROWS*COLS], int copy[ROWS*COLS]);

#endif // GAME_BOARD_H
