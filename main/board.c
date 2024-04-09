//
// Created by wiktoria on 16/12/23.
//


// include required standard C libraries
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

// include dependency sources
#include "board.h"
#include "alphabeta.h"

bool can_add_coin(const int board[ROWS*COLS], int col) {
    if (col >=0 && col<COLS) {
        if (is_column_full(board,col)) {
            return false;
        } else {
            return true;
        }
    }
    return false;
}

bool is_column_full(const int board[ROWS*COLS], int col) {
    return (board[(ROWS-1)*COLS+col] != EMPTY);
}

void printBoard(const int *board) {
    printf("* | ");
    for (int l = 0; l < COLS; l++) {
        printf("%d ",l);
    } //TOP WITH COLUMN INDICES
    printf("\n");
    for (int l = 0; l < COLS+2; l++) {
        printf("* ");
    } //BREAKLINE TO SEPARATE COLUMN INDICES
    printf("\n");
    for (int k = ROWS-1; k>=0; k--) {
        printf("%d | ",k); //ROW INDEX
        for (int j=0; j<COLS; j++) {
            //printf("%d ",k*COLS+j);
            printf("%d ", board[k*COLS+j]);
            //printf("(%d,%d) ",i,j);
        }
        printf("\n");
    }


}

int filled_level(const int board[ROWS*COLS], int col) {
    int top = 0;
    int field = board[top*COLS+col];
    while (field != EMPTY) {
        top++;
        field = board[top*COLS+col];
    }
    return top;
}

void add_coin(int *board,int col, int turn) {
    if (is_column_full(board, col)) {
        return;
    } else {
        int row_id = filled_level(board,col);
        board[row_id*COLS+col] = turn;
        turn = 5-turn;
    }
}

bool check_four(const int board[ROWS*COLS]) {
    char *message_com = malloc(sizeof(char)*50);
    for (int idx=0; idx<ROWS*COLS; idx++) {
        if (board[idx] != EMPTY) {
            int col = idx % COLS;
            int row = (idx - col) / COLS;
            int count = 0;
            //VERTICAL CHECK
            if (row < ROWS-3) {
                // PART 1: collect sum for the four in row from the idx
                for (int f = 0; f < 4; f++) {
                    if (board[idx+f*COLS] == EMPTY) {
                        count=0;
                        break;
                    }
                    count += board[idx + f * COLS];
                }
                // PART 2: check the sum for the four
                if (count == 4 * HUMAN || count == 4 * COMPUTER) {
                    printf("win is vertical\n");
                    return true;
                }
            }
            count = 0; //reset the count
            //HORIZONTAL CHECK
            if (col < 4) {
                // PART 1: collect sum for the four in row from the idx
                for (int f = 0; f < 4; f++) {
                    if (board[idx+f] == EMPTY) {
                        count=0;
                        break;
                    }
                    count += board[idx+f];
                }
                // PART 2: check the sum for the four
                if (count == 4 * HUMAN || count == 4 * COMPUTER) {
                    printf("win is horizontal\n");
                    return true;
                }
            }
            count = 0; //reset the count
            //POSITIVE DIAGONAL CHECK
            if (col-row < 4 && col-row > -3) {
                // CASE 1: BOTTOM SIDE
                if (row < 4 && col < 4) {
                    // PART 1: collect sum for the four in row from the idx
                    for (int i=0; i <= row; i++) {
                        for (int f=0; f < 4; f++) {
                            if (board[idx-i*(COLS+1)+f*(COLS+1)] == EMPTY) {
                                count=0;
                                break;
                            }
                            count += board[idx-i*(COLS+1)+f*(COLS+1)];
                        }
                        count = 0;
                        // PART 2: check the sum for the four
                        if (count == 4 * HUMAN || count == 4 * COMPUTER) {
                            printf("win is positive bottom\n");
                            return true;
                        }
                        count = 0;
                    }
                }
                count=0;
                // CASE 2: TOP SIDE
                if (row > 2 && col > 3) {
                    // PART 1: collect sum for the four in row from the idx
                    for (int i = 0; i < ROWS-row; i++) {
                        // PART 1: collect sum for the four in row from the idx
                        for (int f = 0; f < 4; f++) {
                            if (board[idx+i*(COLS+1)-f*(COLS+1)] == EMPTY) {
                                count=0;
                                break;
                            }
                            count += board[idx+i*(COLS+1)-f*(COLS+1)];
                        }
                        // PART 2: check the sum for the four
                        if (count == 4 * HUMAN || count == 4 * COMPUTER) {
                            printf("win is positive top:\n"
                                   "board[%d]=%d\n"
                                   "board[%d]=%d\n"
                                   "board[%d]=%d\n"
                                   "board[%d]=%d\n",
                                   idx, board[idx],
                                   idx+i*(COLS+1)-1*(COLS+1), board[idx+i*(COLS+1)-1*(COLS+1)],
                                   idx+i*(COLS+1)-2*(COLS+1), board[idx+i*(COLS+1)-2*(COLS+1)],
                                   idx+i*(COLS+1)-3*(COLS+1), board[idx+i*(COLS+1)-3*(COLS+1)]);
                            return true;
                        }
                        count = 0;
                    }
                }
            }
            count = 0; //reset the count
            //NEGATIVE DIAGONAL
            if (col+row > 2 && col+row < 9) {
                // CASE 1: BOTTOM SIDE
                if (row < 4) {
                    // PART 1: collect sum for the four in row from the idx
                    for (int i = 0; i <= row; i++) {
                        for (int f = 0; f < 4; f++) {
                            if (board[idx-i*(COLS-1)+f*(COLS-1)] == EMPTY) {
                                count=0;
                                break;
                            }
                            count += board[idx-i*(COLS-1)+f*(COLS-1)];
                        }
                        // PART 2: check the sum for the four
                        if (count == 4 * HUMAN || count == 4 * COMPUTER) {
                            printf("win is negative bottom\n");
                            return true;
                        }
                        count = 0;
                    }
                }
                count=0;
                // CASE 2: TOP SIDE
                if (row > 2) {
                    // PART 1: collect sum for the four in row from the idx
                    for (int i=0; i < ROWS-row; i++) {
                        for (int f = 0; f < 4; f++) {
                            if (board[(idx-i*(COLS-1))+f*(COLS-1)] == EMPTY) {
                                count=0;
                                break;
                            }
                            count += board[(idx-i*(COLS-1))+f*(COLS-1)];
                        }
                        if (count == 4 * HUMAN || count == 4 * COMPUTER) {
                            printf("win is negative top\n");
                            return true;
                        }
                        count = 0; //reset count for the next loop
                    }
                }
            }

        }
    }
    free(message_com);
    return false;
}

bool is_it_win_move(const int board[ROWS*COLS], int col,int turn) {
    bool a = check_vertical(board,col,turn);
    //printf("is it vertical? %d\n",a);
    bool b = check_horizontal(board,col,turn);
    //printf("is it horizontal? %d\n",b);
    bool c = check_diagonal_positive(board,col,turn);
    //printf("is it diagonal positive? %d\n",c);
    bool d = check_diagonal_negative(board,col,turn);
    //printf("is it diagonal negative? %d\n",d);
    //printBoard(board);
    //printf("a:%d, b:%d, c:%d, d:%d\n",a,b,c,d);
    return  a || b || c || d;
}
bool check_vertical(const int board[ROWS*COLS], int col,int turn) {
    int row = filled_level(board, col);
    int idx = row*COLS+col;
    int count= 0 ;
    if (row>=3 && row<ROWS) {
        count = 0;
        for (int f=1; f<4; f++) {
            count += board[idx-f*COLS];
        }
        //printf("count: %d+%d+%d=%d\n",board[idx],board[idx-7],board[idx-14],board[idx-21]);
        if (count == 3*HUMAN || count == 3*COMPUTER) { //6 || 9
            if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                return true;
            }
        }
    }
    return false;
}
bool check_horizontal(const int board[ROWS*COLS], int col,int turn) {
    int row = filled_level(board,col); int idx = row*COLS+col;
    //printf("row is %d, idx is %d\n",row, idx);
    int count= 0 ;
    if (col < 4) {
        for (int i=0; i<=col; i++) {
            //printf("horizontal for %d shift:", i);
            for (int f=0; f<4; f++) {
                count += board[idx-i+f];
                //printf(" board[%d]=%d,",idx-i+f,board[idx-i+f]);
            }
            //printf(" sum+turn=%d\n", count + turn);
            if (count == 3*HUMAN || count == 3*COMPUTER) { //6 || 9
                if (row > 0 && board[(row-1)*COLS+count] != EMPTY) {
                    if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                        return true;
                    }
                }
                if (row == 0) {
                    if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                        return true;
                    }
                }
            }
            count = 0;
        }
    }
    if (col > 2) {
        for (int i=0; i<=COLS-col; i++) {
            for (int f=0; f<4; f++) {
                count += board[idx+i-f];
            }
            //printf("row: %d %d %d %d\n",idx+i,idx+i-1,idx+i-2,idx+i-3);
            //printf("\tcount: %d+%d+%d+%d=%d + turn %d\n",board[idx+i],board[idx+i-1],board[idx+i-2],board[idx+i-3],count, turn);
            if (count == 3*HUMAN || count == 3*COMPUTER) { //6 || 9
                if (row > 0 && board[(row-1)*COLS+count] != EMPTY) {
                    if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                        return true;
                    }
                }
                if (row == 0) {
                    if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                        return true;
                    }
                }
            }
            count=0;
        }
    }
    return false;
}
bool check_diagonal_positive(const int board[ROWS*COLS], int col,int turn) {
    int row = filled_level(board,col);
    int idx = row*COLS+col;
    int count = 0;
    if (col - row < 4 && col - row > -3) {
        if (row < 4) {
            for (int i = 0; i <= row; i++) {
                //printf("for %d idxs:",i);
                for (int f = 0; f < 4; f++) {
                    count += board[idx - i * (COLS + 1) + f * (COLS + 1)];
                    //printf(" board[%d]=%d,",idx-i*(COLS+1)+f*(COLS+1),board[idx-i*(COLS+1)+f*(COLS+1)]);
                }
                //printf("\n");
                //printf("\tsum+turn=%d\n",count+turn);
                if (count == 3*HUMAN || count == 3*COMPUTER) { //6 || 9
                    if (row > 0 && board[(row-1)*COLS+count] != EMPTY) {
                        if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                            return true;
                        }
                    }
                    if (row == 0) {
                        if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                            return true;
                        }
                    }
                }
                count=0;
            }
        } else if (row > 2) {
            for (int i = 0; i < ROWS - row; i++) { //6-5=1
                //printf("for %d idxs:",ROWS-i);
                for (int f = 0; f < 4; f++) {
                    count += board[idx + i * (COLS + 1) - f * (COLS + 1)];
                    // 33, 25, 17, 9
                    // 41, 33, 25, 17
                }
                /*printf("idx: %d: %d+%d+%d+%d=%d + turn %d\n",
                       idx,
                       board[idx+i*(COLS+1)],
                       board[idx+i*(COLS+1)-(COLS+1)],
                       board[idx+i*(COLS+1)-2*(COLS+1)],
                       board[idx+i*(COLS+1)-3*(COLS+1)],
                       count, turn);*/
                if (count == 3*HUMAN || count == 3*COMPUTER) { //6 || 9
                    if (row > 0 && board[(row-1)*COLS+col] != EMPTY) {
                        if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                            return true;
                        }
                    }
                    if (row == 0) {
                        if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                            return true;
                        }
                    }
                }
                count=0;
            }
        }
    }
    return false;
}
bool check_diagonal_negative (const int board[ROWS*COLS], int col,int turn) {
    int row = filled_level(board,col);
    int idx = row*COLS+col;
    int count = 0;
    if (col+row > 2 && col+row < 9) {
        if (row < 4) {
            for (int i = 0; i <= row; i++) {
                //printf("diag negative for %d shift:", i);
                for (int f = 0; f < 4; f++) {
                    count += board[idx - i * (COLS - 1) + f * (COLS - 1)];
                    //printf(" board[%d]=%d,",idx-i*(COLS-1)+f*(COLS-1),board[idx-i*(COLS-1)+f*(COLS-1)]);
                }
                //printf(" sum=%d\t sum+turn=%d\n",count, count + turn);
                if (count == 3*HUMAN || count == 3*COMPUTER) { //6 || 9
                    if (row > 0 && board[(row-1)*COLS+count] != EMPTY) {
                        if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                            return true;
                        }
                    }
                    if (row == 0) {
                        if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                            return true;
                        }
                    }
                }
                count=0;
            }
        } else if (row > 2) {
            for (int i = 0; i < ROWS - row; i++) {
                //printf("%d shift:", i);
                for (int f = 0; f < 4; f++) {
                    count += board[(idx - i * (COLS - 1)) + f * (COLS - 1)];
                    //printf(" board[%d]=%d,",idx-i*(COLS-1)+f*(COLS-1),board[idx-i*(COLS-1)+f*(COLS-1)]);
                }
                //printf(" sum+turn=%d\n", count + turn);
                if (count == 3*HUMAN || count == 3*COMPUTER) { //6 || 9
                    if (row > 0 && board[(row-1)*COLS+count] != EMPTY) {
                        if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                            return true;
                        }
                    }
                    if (row == 0) {
                        if (count + turn == 4*HUMAN || count + turn == 4*COMPUTER) { //8 || 12
                            return true;
                        }
                    }
                }
                count=0;
            }
        }
    }
    return false;
}

void initializeBoard(int *board) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board[i*COLS+j] = EMPTY;
        }
    }
}

int getNoMoves(const int board[ROWS*COLS]) {
    int count=0;
    for (int i=0; i<ROWS*COLS; i++) {
        if (board[i] == 2 || board[i] == 3) {
            count++;
        }
    }
    return count;
}

int evaluate(const int board[ROWS*COLS]) {
    if (getNoMoves(board) == 40) {
        return 0;
    }
    int score = 0;
    score = evaluate_vertical(board);
    //printf("after vertical: %d\n",score);
    score += evaluate_horizontal(board);
    //printf("after horizontal: %d\n",score);
    score += evaluate_positive_diagonal(board);
    //printf("after diagonal positive: %d\n",score);
    score += evaluate_negative_diagonal(board);
    //printf("after diagonal negative: %d\n",score);
    return score;
}
int evaluate_vertical(const int board[ROWS*COLS]) {
    int score = 0;
    for (int col=0; col<COLS; col++) {
        int consecutive_count = 0;
        for (int row=0; row<ROWS; row++) {
            if (board[row*COLS+col] == COMPUTER) {
                consecutive_count++;
            } else {
                if (consecutive_count == 3) {
                    score += HIGH_SCORE;
                } else if (consecutive_count == 2) {
                    score += MEDIUM_SCORE;
                }
                consecutive_count = 0;
            }
        }
    }
    return score;
}
int evaluate_horizontal(const int board[ROWS*COLS]) {
    int score=0;
    for (int idx=0; idx<ROWS*COLS; idx++) {
        if (board[idx] == COMPUTER) {
            if (idx%COLS + 3 < COLS) {
                if (board[idx] == board[idx+1]) {
                    if (board[idx] == board[idx+2]) {
                        score += HIGH_SCORE;
                        idx += 2;
                    } else {
                        score += MEDIUM_SCORE;
                    }
                }
            }
        }
    }
    return score;
}
int evaluate_positive_diagonal(const int board[ROWS*COLS]) {
    int score=0;
    for (int col=0; col < COLS-3; col++) {
        int consecutive_count = 0;
        for (int idx = col; idx < ROWS*COLS; idx+=COLS+1) {
            if (board[idx] == COMPUTER) {
                consecutive_count++;
            } else {
                if (consecutive_count == 3) {
                    score += HIGH_SCORE;
                } else if (consecutive_count == 2) {
                    score += MEDIUM_SCORE;
                }
                consecutive_count = 0;
            }
        }
    }
    for (int row = 1; row<ROWS-3; row++) {
        int consecutive_count = 0;
        for (int idx = row*COLS; idx < ROWS*COLS; idx+=COLS+1) {
            if (board[idx] == COMPUTER) {
                consecutive_count++;
            } else {
                if (consecutive_count == 3) {
                    score += HIGH_SCORE;
                } else if (consecutive_count == 2) {
                    score += MEDIUM_SCORE;
                }
                consecutive_count = 0;
            }
        }
    }
    return score;
}
int evaluate_negative_diagonal(const int board[ROWS*COLS]) {
    int score=0;
    for (int col=COLS-1; col>=3; col--) {
        int consecutive_count = 0;
        for (int idx = col; idx < ROWS*COLS; idx += COLS-1) {
            if (board[idx] == COMPUTER) {
                consecutive_count++;
            } else {
                if (consecutive_count == 3) {
                    score += HIGH_SCORE;
                } else if (consecutive_count == 2) {
                    score += MEDIUM_SCORE;
                }
                consecutive_count = 0;
            }
        }
    }
    for (int row = 1; row<ROWS-3; row++) {
        int consecutive_count = 0;
        for (int idx = row*(COLS+1)-1; idx < ROWS*COLS; idx += COLS-1) {
            if (board[idx] == COMPUTER) {
                consecutive_count++;
            } else {
                if (consecutive_count == 3) {
                    score += HIGH_SCORE;
                } else if (consecutive_count == 2) {
                    score += MEDIUM_SCORE;
                }
                consecutive_count = 0;
            }
        }
    }
    return score;
}

move negamax(int board[ROWS*COLS], int depth, int turn) {
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
            move next_move = negamax(copy,depth-1,5-turn);
            next_move.score = -next_move.score;
            if (test_move.score <= next_move.score) {
                test_move.score = next_move.score;
                test_move.col = next_move.col;
            }
        }
    }
    free(copy);
    return test_move;
}

void copy_board(const int board[ROWS*COLS], int copy[ROWS*COLS]) {
    memcpy(copy,board, sizeof(*board));
    for (int idx=0; idx<ROWS*COLS; idx++) {
        copy[idx] = board[idx];
    }
}

bool test_copy(const int board[ROWS*COLS], int copy[ROWS*COLS]) {
    for (int i=0; i<ROWS*COLS; i++) {
        if (board[i] != copy[i]) {
            return false;
        }
    }
    return true;
}

/* NOT FULLY DELETING FROM HERE BC TESTING FAILS
int *get_exploration_order(int width) {
    int *order = malloc(width* sizeof(int));
    if (order == NULL) {
        return NULL; //handling error in memory allocation
    }
    //printf("order: ");
    for (int i=0; i<width; i++) {
        order[i] = width/2 + (1-2*(i%2))*(i+1)/2;
        //printf("%d ",order[i]);
    }
    //printf("\n");

    return order;
}
*/
