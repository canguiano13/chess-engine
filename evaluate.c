//class helps evaluate position scores
#include <stdio.h>
#include "defs.h"

//holds specified values for pawns based on their current (64-indexed) square
//current evaluation values pawns in the center and on the end ranks
const int PawnTable[64] = {
 0,  0,  0,  0,  0,  0,  0,  0,
10, 10,  0,-10,-10,  0, 10, 10,
 5,  0,  0,  5,  5,  0,  0,  5,
 0,  0, 10, 20, 20, 10,  0,  0,
 5,  5,  5, 10, 10,  5,  5,  5, 
10, 10, 10, 20, 20, 10, 10, 10,
20, 20, 20, 30, 30, 20, 20, 20,
0,  0,  0,  0,  0,  0,  0,  0
};

//holds specified values for knights based on their current (64-indexed) square
//current evaluation values knights in the center of the board and away from the corners
const int KnightTable[64] = {
-10,-10, 0,  0,  0,  0,-10,-10,
10,  10, 0,  5,  5,  0, 10, 10,
 0,  0, 10, 10, 10, 10,  0,  0,
 0,  5, 10, 20, 20, 10,  5,  0,
 5, 10, 15, 20, 20, 15, 10,  5, 
 5, 10, 15, 20, 20, 15, 10,  5, 
 0,  0,  5, 10, 10,  5,  0,  0,
-3,  0,  0,  0,  0,  0,  0, -3
};

//holds specified values for bishops based on their current (64-indexed) square
//current evaluation values bishops towards the center of the board, taking up several diagonals
const int BishopTable[64] = {
 0,  0,-10,  0,  0,-10,  0,  0,
 0,  0,  0, 10, 10,  0,  0,  0,
 0,  0, 10, 15, 15, 10,  0,  0,
 0, 10, 15, 20, 20, 15, 10,  0,
 0, 10, 15, 20, 20, 15, 10,  0, 
 0,  0, 10, 15, 15, 10,  0,  0,
 0,  0,  0, 10, 10, 0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0
};

//holds specified values for rooks based on their current (64-indexed) square
//current evaluation values rooks in files C-F, and especially on the 7th rank
const int RookTable[64] = {
 0,  0,  5, 10, 10,  5,  0,  0,
 0,  0,  5, 10, 10,  5,  0,  0,
 0,  0,  5, 10, 10,  5,  0,  0,
 0,  0,  5, 10, 10,  5,  0,  0,
 0,  0,  5, 10, 10,  5,  0,  0, 
 0,  0,  5, 10, 10,  5,  0,  0,
25, 25, 25, 25, 25, 25, 25, 25,
 0,  0,  5, 10, 10,  5,  0,  0
};


//determine the static evaluation score of the current position
//TODO create more sophisticated evaluation system
int EvalPosition(const S_BOARD *pos){
    int pce;
    int pceNum;
    int sq;
    int score = pos->material[WHITE] - pos->material[BLACK];

    //evaluate pawns
    pce = wP;
    for(pceNum = wP; pceNum <  pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score += PawnTable[SQ64(sq)];
    }
    pce = bP;
    for(pceNum = bP; pceNum < pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score -= PawnTable[MIRROR64( SQ64(sq) )];
    }

    //evaluate knights
    pce = wN;
    for(pceNum = wN; pceNum <  pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score += KnightTable[SQ64(sq)];
    }
    pce = bN;
    for(pceNum = bN; pceNum < pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score -= KnightTable[MIRROR64( SQ64(sq) )];
    }

    //evaluate bishops
    pce = wB;
    for(pceNum = wB; pceNum <  pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score += BishopTable[SQ64(sq)];
    }
    pce = bB;
    for(pceNum = bB; pceNum < pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score -= BishopTable[MIRROR64( SQ64(sq) )];
    }

    //evaluate rooks
    pce = wR;
    for(pceNum = wR; pceNum <  pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score += RookTable[SQ64(sq)];
    }
    pce = bR;
    for(pceNum = bR; pceNum < pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score -= RookTable[MIRROR64( SQ64(sq) )];
    }

    //return static evaluation score from active side's perspective
    if(pos->side == WHITE){
        return score;
    }
    return -score;
}