//used to set modify the current chess board position

#include <stdio.h>
#include "defs.h"

void ResetBoard(S_BOARD *pos){
    int index = 0;

    //mark all squares in full-board representation as offboard
    for(index = 0; index < BRD_SQ_NUM; index++){
        pos->pieces[index] = OFFBOARD;
    }
    //set on-board squares to empty
    for(index = 0; index < 64; index++){
        pos->pieces[SQ120(index)] = EMPTY;
    }
    //reset individual piece trackers
    for(index = 0; index < 3; index++){
        pos->bigPce[index] = 0;
        pos->majPce[index] = 0;
        pos->minPce[index] = 0;
        pos->pawns[index] = 0ULL;
    }
    //reset overall piece counter
    for(index = 0; index < 13; index++){
        pos->pceNum[index] = 0;
    }
    //remove kings from board
    pos->KingSQ[WHITE] = pos->KingSQ[BLACK] = NO_SQ;
    //reset castling permissions, fifty move tracker, enPassant square
    pos->side = BOTH;
    pos->enPassant = NO_SQ;
    pos->fiftyMove = 0;
    pos->castlePerm = 0;
    //reset current/previous move counters
    pos->ply = 0;
    pos->hisPly = 0;
    //reset positionID
    pos->posKey = 0;

}