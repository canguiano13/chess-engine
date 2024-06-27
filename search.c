#include <stdio.h>
#include "defs.h"

//check for timer end or interrupt
static void CheckUp(){

}

//detect repetition of position
static int IsRepetition(const S_BOARD *pos){
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

//prepare for new search
static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info){
    return;
}

//perform iteration of alpha-beta search
static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull){
    return -1;
}

//quiescent search helps alleviate horizon effect
static int Quiescence(int alpha, int beta, S_BOARD *POS, S_SEARCHINFO *info){
    return 0;
}

//perform iterative deepening alpha-beta search to find next move
void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info){

}