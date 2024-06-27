#include <stdio.h>
#include "defs.h"

int IsRepetition(const S_BOARD *pos){
    int index;
    //starting from last non-repeatable position, search for repetition of moves
    for(index = (pos->hisPly - pos->fiftyMove) ; index < pos->hisPly-1; index++){
        ASSERT(index >= 0 && index < MAX_GAME_MOVES)
        if(pos->posKey == pos->history[index].posKey){
            return TRUE;
        }
    }
    return FALSE;
}

void SearchPosition(S_BOARD *pos){

}