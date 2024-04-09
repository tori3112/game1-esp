//
// Created by wiktoria on 30/12/23.
//

// include standard C libraries
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
// include project dependencies
#include "board.h"
#include "logger.h"
#include "alphabeta.h"
#include "random_board.c"

int main() {
    int board[ROWS*COLS];
    initializeBoard(board);

    logMessage("the game is initiated");
    bool game_not_over = true;
    int turn = HUMAN; //1 for human, 2 for computer
        while (game_not_over) {
        switch (turn) {
            case HUMAN:
                logMessage("human input turn"); //GET HUMAN INPUT
                printf("Provide column no for input: ");
                int col_idx;
                scanf("%i", &col_idx);

                bool coin_input = !can_add_coin(board,col_idx); //VALIDATE HUMAN INPUT
                while (coin_input) {
                    printf("Invalid input, try again\n");
                    printf("Provide column no for input: ");
                    scanf("%i", &col_idx);
                    coin_input = can_add_coin(board, col_idx);
                }
                add_coin(board,col_idx, HUMAN);        //PLAY HUMAN INPUT
                printBoard(board);

                char *message = malloc(sizeof(char)*50);    //LOG HUMAN INPUT
                snprintf(message,50,"human turn: coin added to a column no %i",col_idx);
                logMessage(message);
                free(message);
                game_not_over = !check_four(board);           //CHECK GAME OVER
                if (!game_not_over) {
                    logMessage("four in a row found! you win");
                    printf("four in a row found! you win\n");
                } else {
                    logMessage("no four in a row, game continues");
                }
                turn=COMPUTER;     //CHANGE TURN
                break;

            case COMPUTER:
                logMessage("computer input turn");

                //move next_move = negamax(board,10, COMPUTER);       //FIND COMPUTER INPUT W/OUT PRUNING
                move next_move = negamax_ab(board,INT_MIN,INT_MAX,10,COMPUTER);

                char *message_score = malloc(sizeof(char)*50);     //LOG COMPUTER INPUT SCORE
                snprintf(message_score,60,"computer turn: best score calculated = %i, into column %d",
                         next_move.score, next_move.col);
                logMessage(message_score);
                free(message_score);

                add_coin(board,next_move.col,COMPUTER);                 //PLAY COMPUTER INPUT
                printf("\n");
                printBoard(board);
                int get_level = filled_level(board,next_move.col);
                if (board[(get_level-1)*COLS+next_move.col] == COMPUTER) {
                    logMessage("computer input succeeded");     //LOG COMPUTER INPUT
                } else {
                    exit(EXIT_FAILURE);
                }

                char *message_com = malloc(sizeof(char)*50);
                snprintf(message_com,50,"computer turn: coin added to a column no %i",next_move.col);
                logMessage(message_com);
                free(message_com);

                printf("computer input into column %d\n",next_move.col);

                game_not_over = !check_four(board);           //CHECK GAME OVER
                if (!game_not_over) {
                    logMessage("four in a row found! computer wins");
                    printf("four in a row found! computer wins\n");
                }  else {
                    logMessage("no four in a row, game continues");
                }
                turn = HUMAN;       //CHANGE TURN
                break;
            default:
                logMessage("shouldn't go into default");
        }
        //game_not_over = false; //this stops from infinite loop for now
    }
    //LAST LINE OF WHILE LOOP
    return 0;
}