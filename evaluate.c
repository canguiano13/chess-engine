//class helps evaluate position scores
#include <stdio.h>
#include "defs.h"

const int PawnIsolated = -10;
const int PawnPassed[8] = {0, 5, 10, 20, 35, 60, 100, 200};

//positional evaluation markers
const int RookOpenFile = 10;
const int RookSemiOpenFile = 5;
const int QueenOpenFile = 5;
const int QueenSemiOpenFile = 3;
const int BishopPair = 30;

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
 0,  0, 10, 20, 20, 10,  0,  0,
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

//holds specified values for the king in the endgame based on its current square
//current evaluation values active kings towards the center of the board
const int KingE[64] = {	
	-50, -10,  0,  0,  0,  0,-10,-50,
	-10,   0, 10, 10, 10, 10,  0,-10,
	  0 , 10, 20, 20, 20, 20, 10,  0,
	  0,  10, 20, 40, 40, 20, 10,  0,
	  0,  10, 20, 40, 40, 20, 10,  0,
	  0,  10, 20, 20, 20, 20, 10,  0,
	-10,   0, 10, 10, 10, 10,  0,-10,
    -50, -10,  0,  0,  0,  0,-10,-50
};

//holds evaluation for kings in non-endgame situations
//current evaluation values safe kings away from the center
const int KingO[64] = {
 5,  5,  5,  -5, -5,  0, 5,  5,
-10,-10,-10,-10,-10,-10,-10,-10,
-30,-30,-30,-30,-30,-30,-30,-30,
-70,-70,-70,-70,-70,-70,-70,-70,
-70,-70,-70,-70,-70,-70,-70,-70, 
-70,-70,-70,-70,-70,-70,-70,-70,
-70,-70,-70,-70,-70,-70,-70,-70,
-70,-70,-70,-70,-70,-70,-70,-70
};

//identify theoretical draws
int MaterialDraw(const S_BOARD *pos){
    //if no rooks or queens
    if(!pos->pceNum[wR] && !pos->pceNum[bR] && !pos->pceNum[wQ] && !pos->pceNum[bQ]){
        //if no bishops
        if(!pos->pceNum[bB] && !pos->pceNum[wB]){
            //check for 2N
            if(pos->pceNum[wN] < 3 && pos->pceNum[bN] < 3) {return TRUE;}
        }
        //if no knights
        else if (!pos->pceNum[wN] && !pos->pceNum[bN]){
            //check for two bishops
            if(abs(pos->pceNum[wB] - pos->pceNum[bB]) < 2) {return TRUE;}
        }
        //check for bishop + knight
        else if((pos->pceNum[wN] < 3 && !pos->pceNum[wB]) || (pos->pceNum[wB] == 1 && !pos->pceNum[wN])){
            if((pos->pceNum[bN] < 3 && !pos->pceNum[bB]) || (pos->pceNum[bB] && !pos->pceNum[bN])) {return TRUE;}
        }
    }
    //check for theoretical draws
    else if(!pos->pceNum[wQ] && !pos->pceNum[bQ]){
        if(pos->pceNum[wR] == 1 && pos->pceNum[bR] == 1){
            //R vs R+B or R vs R+N
            if((pos->pceNum[wN] + pos->pceNum[wB]) < 2 && (pos->pceNum[bN] + pos->pceNum[bB]) < 2) {return TRUE;}
        }
        //R vs. B or R vs. N or R vs. B+N
        else if(pos->pceNum[wR] == 1 && !pos->pceNum[bR]){
            if((pos->pceNum[wN] + pos->pceNum[wB]) == 0 && (((pos->pceNum[bN] + pos->pceNum[bB]) == 1) || ((pos->pceNum[bN] + pos->pceNum[bB]) == 2))) {return TRUE;}   
        }
        else if(pos->pceNum[bR] == 1 && !pos->pceNum[wR]){
            if((pos->pceNum[bN] + pos->pceNum[bB]) == 0 && (((pos->pceNum[wN] + pos->pceNum[wB]) == 1) || ((pos->pceNum[wN] + pos->pceNum[wB]) == 2))) {return TRUE;}
        }
    }
    return FALSE;
}

//determine the static evaluation score of the current position
//TODO create more sophisticated evaluation system
int EvalPosition(const S_BOARD *pos){
    int pce;
    int pceNum;
    int sq;
    int score = pos->material[WHITE] - pos->material[BLACK];

    //check for theoretical draws
    if((pos->pceNum[wP] == 0 && pos->pceNum[bP] == 0) && MaterialDraw(pos) == TRUE){return 0;}

    //evaluate pawns
    pce = wP;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score += PawnTable[SQ64(sq)];
        //check if pawn is isolated
        if( (IsolatedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0){
            //printf("wP Iso: %s\n", PrSq(sq));
            score += PawnIsolated;
        }
        //check if pawn is a passed pawn
        if( (WhitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0){
            //printf("wP Passed: %s\n", PrSq(sq));
            score += PawnPassed[RanksBrd[sq]];
        }
    }
    pce = bP;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score -= PawnTable[MIRROR64( SQ64(sq) )];

        //check if pawn is isolated
        if( (IsolatedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0){
            //printf("bP Iso: %s\n", PrSq(sq));
            score -= PawnIsolated;
        }
        //check if pawn is a passed pawn
        if( (BlackPassedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0){
            //printf("bP Passed: %s\n", PrSq(sq));
            score -= PawnPassed[7-RanksBrd[sq]];
        }
    }

    //evaluate knights
    pce = wN;
    for(pceNum = 0; pceNum <  pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score += KnightTable[SQ64(sq)];
    }
    pce = bN;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score -= KnightTable[MIRROR64( SQ64(sq) )];
    }

    //evaluate bishops
    pce = wB;
    for(pceNum = 0; pceNum <  pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score += BishopTable[SQ64(sq)];
    }
    pce = bB;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score -= BishopTable[MIRROR64( SQ64(sq) )];
    }

    //evaluate rooks
    pce = wR;
    for(pceNum = 0; pceNum <  pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score += RookTable[SQ64(sq)];
        //check for open or semi-open file
        if(! (pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]]) ){
            score += RookOpenFile;
        }
        else if (! (pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]]) ){
            score+=RookSemiOpenFile;
        }
    }
    pce = bR;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        score -= RookTable[MIRROR64( SQ64(sq) )];

        //check for open or semi-open file
        if(! (pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]]) ){
            score -= RookOpenFile;
        }
        else if (! (pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]]) ){
            score-=RookSemiOpenFile;
        }
    }

    //evaluate queens
    pce = wQ;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        //check or open or semi-open files
        if(! (pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]]) ){
            score += QueenOpenFile;
        }
        else if (! (pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]]) ){
            score+=QueenSemiOpenFile;
        }
    }
    pce = bQ;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++){
        sq = pos->pList[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        //check for open or semi-open file
        if(! (pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]]) ){
            score += QueenOpenFile;
        }
        else if (! (pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]]) ){
            score += QueenSemiOpenFile;
        }
    }

    //evaluate kings
    pce = wK;
    sq = pos->pList[pce][0];
    //if there is less than endgame material threshold, evaluate king's position based on endgame scores
    if(pos->material[BLACK] < ENDGAME_MAT){
        score += KingE[SQ64(sq)];
    }
    else{
        score += KingO[SQ64(sq)];
    }
    pce = bK;
    sq = pos->pList[pce][0];
    if(pos->material[WHITE] < ENDGAME_MAT){
        score -= KingE[MIRROR64(SQ64(sq))];
    }
    else{
        score -= KingO[MIRROR64(SQ64(sq))];
    }

    //check for bishop pair
    if(pos->pceNum[wB] >= 2) score += BishopPair;
    if(pos->pceNum[bB] >= 2) score -= BishopPair;

    //return static evaluation score from active side's perspective
    if(pos->side == WHITE){
        return score;
    }
    return -score;
}



