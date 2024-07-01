#include "defs.h"

//arrays that determine, given an index of a square on the chessboard, the indices of potential squares each piece can move to
const int KnDir[8] = { -8, -19, -21, -12, 8, 19, 21, 12 }; 
const int RkDir[8] = { -1, -10, 1, 10 }; 
const int BiDir[8] = { -9, -11, 11, 9 }; 
const int KiDir[8] = { -1, -10, 1, 10, -9, -11, 11, 9 }; 

//returns 1 if a square on the chess board is attacked by a piece
int SqAttacked(const int sq, const int side, const S_BOARD *pos){
    int pce, index, t_sq, dir;

    ASSERT(SqOnBoard(sq));
    ASSERT(SideValid(side));
    ASSERT(CheckBoard(pos));

    //pawns
    if(side == WHITE){
        if(pos->pieces[sq-11] == wP || pos->pieces[sq-9] == wP){
            return TRUE;
        }
    }
    else{
        if(pos->pieces[sq+11] == bP || pos->pieces[sq+9] == bP){
            return TRUE;
        }
    }

    //knights
    for(index = 0; index < 8; index++){
        pce = pos->pieces[sq + KnDir[index]];
        ASSERT( PieceValidEmptyOffboard(pce) );
        if(pce != OFFBOARD && IsKN(pce) && PieceCol[pce] == side){
            return TRUE;
        }
    }

    //rooks, queens
    for(index = 0; index < 4; index++){
        dir = RkDir[index];
        t_sq = sq + dir;
        pce = pos->pieces[t_sq];
        //check all possible on-board squares in the same direction
        while(pce != OFFBOARD){
            if(pce != EMPTY){
                if(IsRQ(pce) && PieceCol[pce] == side){
                    return TRUE;
                }
                break;
            }
            t_sq = t_sq + dir;
            pce = pos->pieces[t_sq];
        }
    }

    //bishops, queens
    for(index = 0; index < 4; index++){
        dir = BiDir[index];
        t_sq = sq + dir;
        pce = pos->pieces[t_sq];
        //check all possible on-board squares in the same direction
        while(pce != OFFBOARD){
            if(pce != EMPTY){
                if(IsBQ(pce) && PieceCol[pce] == side){
                    return TRUE;
                }
                break;
            }
            t_sq = t_sq + dir;
            pce = pos->pieces[t_sq];
        }
    }

    //kings
    for(index = 0; index < 8; index++){
        pce = pos->pieces[sq + KiDir[index]];
        if(pce != OFFBOARD && IsKI(pce) && PieceCol[pce] == side){
            return TRUE;
        }
    }

    return FALSE;
}