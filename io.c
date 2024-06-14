#include "defs.h"
#include <stdio.h>

//print board square using algebraic notation
char *PrSq(const int sq){
    static char SqStr[3];

    int file = FilesBrd[sq];
    int rank = RanksBrd[sq]; //get file and rank of square

    sprintf(SqStr, "%c%c", ('a'+file), ('1'+rank));

    return SqStr;
}

//print move in algebraic notation
char *PrMove(const int move){
    static char MvStr[6];
    //establish square piece was moved from
    int ff = FilesBrd[(FROMSQ(move))]; 
    int rf = RanksBrd[(FROMSQ(move))];
    //establish square piece was moved to
    int ft = FilesBrd[(TOSQ(move))]; 
    int rt = RanksBrd[(TOSQ(move))];

    int promoted = PROMOTED(move);

    if(promoted){
        char pchar = 'Q'; //assume promotion was to queen, then check for other pieces
        if(IsKN(promoted)){
            pchar = 'N';
        } else if(IsRQ(promoted) && !IsBQ(promoted)){
            pchar = 'R';
        } else if(!IsRQ(promoted) && IsBQ(promoted)){
            pchar = 'B';
        }
        sprintf(MvStr, "%c%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt), pchar);
    } else{
        sprintf(MvStr, "%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt));
    }

    return MvStr;
}

void PrintMoveList(const S_MOVELIST *list){
    int index = 0;
    int score = 0;
    int move = 0;

    printf("MoveList:\n");
    for(index = 0; index < list->count; index++){
        move = list->moves[index].move;
        score = list->moves[index].score;
        printf("Move:%d > %s (score: %d)\n", index+1, PrMove(move), score);
    }
    printf("Total: %d Moves\n\n", list->count);
}