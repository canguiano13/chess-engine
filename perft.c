#include "defs.h"
#include <stdio.h>

long leafNodes;

void Perft(int depth, S_BOARD *pos){
    ASSERT(CheckBoard(pos));

    //if there are no more moves left to evaluate in branch, return control to caller
    if(depth == 0){
        leafNodes++;
        return;
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int MoveNum = 0;
    for(MoveNum = 0; MoveNum < list->count; MoveNum++){
        //if move cannot be made, move to next move in list
        if( !MakeMove(pos, list->moves[MoveNum].move)){
            continue;
        }
        //recursively generate moves for deeper and deeper sequences until all moves are exhausted
        Perft(depth-1, pos);
        TakeMove(pos);
    }

    return;
}

void PerftTest(int depth, S_BOARD *pos){
    ASSERT(CheckBoard(pos));

    //PrintBoard(pos);

    printf("\nStarting Perft Test to Depth: %d...\n", depth);

    leafNodes = 0;

    S_MOVELIST list[1];

    GenerateAllMoves(pos, list);

    int move;
    int MoveNum = 0;
    for(MoveNum = 0; MoveNum < list->count; MoveNum++){
        move = list->moves[MoveNum].move;
        
        if(!MakeMove(pos, move)){
            continue;
        }

        long cumulative = leafNodes;
        Perft(depth-1, pos);
        TakeMove(pos);
        long oldNodes = leafNodes-cumulative;
        printf("Move %d : %s : %1d\n", MoveNum+1, PrMove(move), oldNodes);
    }
    printf("\nTEST COMPLETE: %1d nodes visited.\n", leafNodes);
    return;
}


