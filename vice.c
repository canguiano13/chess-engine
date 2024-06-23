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
#define PERFT_TEST "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

int main(){
    //initialzes game with starting values
    AllInit();
    
    S_BOARD board[1];
    S_MOVELIST list[1];

    ParseFen(PERFT_TEST, board);
    PerftTest(6, board);
    return 0;
}