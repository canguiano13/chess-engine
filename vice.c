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
#include "validate.c"


//Temporary FEN positions to demonstrate methods
#define PERFT_TEST "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1"

int main(){
    //initialzes game with starting values
    AllInit();
    
    S_BOARD board[1];
    S_MOVELIST list[1];
    ParseFen(START_FEN, board);
    
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
        else if (input[0] == 'p'){
            Max = GetPvLine(4, board);
            printf("PvLine of %d moves: ", Max);
            for(PvNum = 0; PvNum < Max; PvNum++){
                Move = board->PvArray[PvNum];
                printf(" %s", PrMove(Move));
            }
            printf("\n");
        }
        else{
            Move = ParseMove(input, board);
            if(Move != NOMOVE){
                StorePvMove(board, Move);

                MakeMove(board, Move);
                // if(IsRepetition(board)){
                //     printf("REP SEEN.\n");
                // }
            }
            else{
                printf("Move Not Parsed: %s\n", input);
            }
        }

        fflush(stdin);
    }
    
    return 0;
}