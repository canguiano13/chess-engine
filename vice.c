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
#include "uci.c"
#include "validate.c"

//Temporary FEN positions to demonstrate methods
#define SEARCH_TEST "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define UCI_TEST "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

int main(){
    //initialzes game with starting values
    AllInit();
    UCI_Loop();
    return 0;
}