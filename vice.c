#include <stdio.h>
#include <stdlib.h>
#include "attack.c"
#include "bitboards.c"
#include "board.c"
#include "data.c"
#include "defs.h"
#include "hashkeys.c"
#include "init.c"
#include "io.c"
#include "makemove.c"
#include "movegen.c"
#include "perft.c"
#include "validate.c"


//Temporary FEN positions to demonstrate methods
#define PERFT_TEST "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1"

int main(){
    //initialzes game with starting values
    AllInit();
    
    S_BOARD board[1];
    S_MOVELIST list[1];
    
    ParseFen(PERFT_TEST, board);
    PrintBoard(board);
    for(int depth = 1; depth <= 6; depth++){
        PerftTest(depth, board);
        printf("\n\n");
    }

    
    return 0;
}