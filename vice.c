#include <stdio.h>
#include <stdlib.h>
#include "attack.c"
#include "bitboards.c"
#include "board.c"
#include "data.c"
#include "defs.h"
#include "evaluate.c"
#include "hashkeys.c"
#include "init.c"
#include "io.c"
#include "makemove.c"
#include "misc.c"
#include "movegen.c"
#include "perft.c"
#include "pvtable.c"
#include "search.c"
#include "validate.c"

//Temporary FEN positions to demonstrate methods
#define SEARCH_TEST "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"

int main(){
    //initialzes game with starting values
    AllInit();
    
    S_BOARD board[1];
    S_MOVELIST list[1];
    S_SEARCHINFO info[1];
    ParseFen(SEARCH_TEST, board);
    
    char input[6];
    int Move = NOMOVE;
    int Max = 0;
    int PvNum = 0;
    while(TRUE){
        PrintBoard(board);
        printf("Enter move > ");
        fgets(input, 6, stdin);

        if(input[0] == 'q'){
            break;
        }
        else if (input[0] == 't'){
            TakeMove(board);
        }
        else if (input[0] == 's'){
            info->depth = 4;
            SearchPosition(board, info);
        }
        else{
            Move = ParseMove(input, board);
            if(Move != NOMOVE){
                StorePvMove(board, Move);
                MakeMove(board, Move);
            }
            else{
                printf("Move Not Parsed: %s\n", input);
            }
        }

        fflush(stdin);
    }
    
    return 0;
}