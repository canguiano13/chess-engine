#include "defs.h"
#include <stdio.h>

//print moves in algebraic notation
char *PrSq(const int sq){
    static char SqStr[3];

    int file = FilesBrd[sq];
    int rank = RanksBrd[sq]; //get file and rank of square

    sprintf(SqStr, "%c%c", ('a'+file), ('1'+rank));

    return SqStr;
}

char *PrMove(const int move){
    static char MvStr[6];
    //establish square piece was moved from
    int ff = FilesBrd[(FROMSQ(move))]; 
    int rf = RanksBrd[(FROMSQ(move))];
    //establish square piece was moved to
    int ft = FilesBrd[(TOSQ(move))]; 
    int rt = FilesBrd[(TOSQ(move))];

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