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
        char pchar = 'q'; //assume promotion was to queen, then check for other pieces
        if(IsKN(promoted)){
            pchar = 'n';
        } else if(IsRQ(promoted) && !IsBQ(promoted)){
            pchar = 'r';
        } else if(!IsRQ(promoted) && IsBQ(promoted)){
            pchar = 'b';
        }
        sprintf(MvStr, "%c%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt), pchar);
    } else{
        sprintf(MvStr, "%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt));
    }

    return MvStr;
}

int ParseMove(char *ptrChar, S_BOARD *pos){
    //validate move passed to function is in bounds
    if(ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
    if(ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
    if(ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
    if(ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;

    //extract start and destination squares from move
    int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
    int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

    ASSERT(SqOnBoard(from) && SqOnBoard(to));

    //generate list of all possible moves in current position and store in list
    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);
    int MoveNum = 0;
    int Move = 0;
    int PromPce = EMPTY;
    //find move passed to function in list of all possible moves
    for(MoveNum = 0; MoveNum < list->count; MoveNum++){
        Move = list->moves[MoveNum].move;
        if(FROMSQ(Move) == from && TOSQ(Move) == to){
            //check that current move matches in promotion value
            PromPce = PROMOTED(Move);
            if(PromPce != EMPTY){
                if(IsRQ(PromPce) && !IsBQ(PromPce) && ptrChar[4] == 'r' ||
                  !IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'b'  ||
                   IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'q'  ||
                   IsKN(PromPce) && ptrChar[4] == 'k'){
                    return Move;
                }
                continue;
            }
            return Move;
            
        }
    }
    //if no such move exists in the position, return empty move value
    return NOMOVE;
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