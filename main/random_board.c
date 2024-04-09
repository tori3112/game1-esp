#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void getRandomPositionString(char position_string[ROWS*COLS]) {
    srand(time(NULL)); //called only once because the need one seed for randomisation
    // EVEN for human input, ODD for computer input
    /*int string_size = (rand()%MAX_SIZE)+1; // from 1 to 42
    if (string_size%2 == 1) {
        string_size--;
    }*/
    int string_size = 7;
    for (int i=0; i<string_size; i++) {
        int move = (rand()%COLS); // from 0 to 6
        position_string[i] = move+'0'; //turn digit to corresponding char
    }
    position_string[string_size] = '\0';

    printf("this is string size:\t%d \n", string_size); //WORKS YAY
    printf("this is position string:\t%s \n",position_string); //WORKS YAY
}

void generateRandomBoard(int board[ROWS*COLS], const char position_notation[ROWS*COLS]) {
    //fill the board based on the Position's string
    for (int idx=0; position_notation[idx]!='\0'; idx++) {
        int move = position_notation[idx] - '0';
        add_coin(board,move,idx%2+1);
    }
}