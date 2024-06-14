#include <stdio.h>
#include "defs.h"

#define MOVE(f, t, ca, pro, fl) ( (f) | ((t)<< 7) | ((ca)<<14) | ((pro) << 20) | (fl))
#define SQOFFBOARD(sq) (FilesBrd[(sq)]==OFFBOARD)

//quiet moves are moves that do not involve captures, so no material is gained
void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list){
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++; //move list pointer
}

//captures allow the side to gain material
void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list){
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++; //move list pointer
}

//EnPassant moves are captures only ave
void AddEnPassant(const S_BOARD *pos, int move, S_MOVELIST *list){
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++; //move list pointer
}

void AddWhitePawnCaptureMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list){
    if(RanksBrd[from] == RANK_7){
        //add all possible promotions
        AddCaptureMove(pos, MOVE(from, to, cap, wQ, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, wR, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, wN, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, wB, 0), list);
    }
    else{
        AddCaptureMove(pos, MOVE(from, to, cap, EMPTY, 0), list);
    }
}
void AddWhitePawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list){
    if(RanksBrd[from] == RANK_7){
        //add all possible promotions
        AddCaptureMove(pos, MOVE(from, to, EMPTY, wQ, 0), list);
        AddCaptureMove(pos, MOVE(from, to, EMPTY, wR, 0), list);
        AddCaptureMove(pos, MOVE(from, to, EMPTY, wN, 0), list);
        AddCaptureMove(pos, MOVE(from, to, EMPTY, wB, 0), list);
    }
    else{
        AddCaptureMove(pos, MOVE(from, to, EMPTY, EMPTY, 0), list);
    }
}

void AddBlackPawnCaptureMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list){
    if(RanksBrd[from] == RANK_2){
        //add all possible promotions
        AddCaptureMove(pos, MOVE(from, to, cap, bQ, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, bR, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, bB, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, bN, 0), list);
    }
    else{
        AddCaptureMove(pos, MOVE(from, to, cap, EMPTY, 0), list);
    }
}
void AddBlackPawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list){
    if(RanksBrd[from] == RANK_2){
        //add all possible promotions
        AddCaptureMove(pos, MOVE(from, to, EMPTY, bQ, 0), list);
        AddCaptureMove(pos, MOVE(from, to, EMPTY, bR, 0), list);
        AddCaptureMove(pos, MOVE(from, to, EMPTY, bN, 0), list);
        AddCaptureMove(pos, MOVE(from, to, EMPTY, bB, 0), list);
    }
    else{
        AddCaptureMove(pos, MOVE(from, to, EMPTY, EMPTY, 0), list);
    }
}

void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list){
    ASSERT(CheckBoard(pos));

    list->count = 0;
    int pce = EMPTY;
    int side = pos->side;
    int sq=0;
    int t_sq=0;
    int pceNum=0;

    if(side == WHITE){
        for(pceNum = 0; pceNum < pos->pceNum[wP]; pceNum++){
            sq = pos->pList[wP][pceNum]; //get square of each white pawn on the board
            ASSERT(SqOnBoard(sq));  //validate square is on the board
            
            //generate quiet white pawn moves
            if(pos->pieces[sq+10] == EMPTY){
                AddWhitePawnMove(pos, sq, sq+10, list);
                //if pawn has not moved yet, it may move two squares if the square is empty
                if(RanksBrd[sq] == RANK_2 && pos->pieces[sq+20] == EMPTY){
                    AddQuietMove(pos, MOVE(sq, (sq+20), EMPTY, EMPTY, MFLAGPS), list);
                }
            }
            //generate pawn captures for white
            if(!SQOFFBOARD(sq+9) && PieceCol[pos->pieces[sq+9]] == BLACK){
                AddWhitePawnCaptureMove(pos, sq, sq+9, pos->pieces[sq+9], list);
            }
            if(!SQOFFBOARD(sq+11) && PieceCol[pos->pieces[sq+11]] == BLACK){
                AddWhitePawnCaptureMove(pos, sq, sq+11, pos->pieces[sq+11], list);
            }
            if(pos->enPassant != NO_SQ){
                //generate en passant moves for white
                if(sq+9 == pos->enPassant){
                    AddCaptureMove(pos, MOVE(sq, sq+9, EMPTY, EMPTY, MFLAGEP), list);
                }
                if(sq+11 == pos->enPassant){
                    AddCaptureMove(pos, MOVE(sq, sq+11, EMPTY, EMPTY, MFLAGEP), list);
                }
            }
        }
    }
    //else black to move
    else{
        for(pceNum = 0; pceNum < pos->pceNum[bP]; pceNum++){
            sq = pos->pList[bP][pceNum]; //get square of each black pawn on the board
            ASSERT(SqOnBoard(sq));  //validate square is on the board

            //generate quiet black pawn moves
            if(pos->pieces[sq-10] == EMPTY){
                AddBlackPawnMove(pos, sq, sq-10, list);
                //if pawn has not moved yet, it may move two squares if the square is empty
                if(RanksBrd[sq] == RANK_7 && pos->pieces[sq-20] == EMPTY){
                    AddQuietMove(pos, MOVE(sq, (sq-20), EMPTY, EMPTY, MFLAGPS), list);
                }
            }
            //generate pawn captures for black
            if(!SQOFFBOARD(sq-9) && PieceCol[pos->pieces[sq-9]] == WHITE){
                AddBlackPawnCaptureMove(pos, sq, sq-9, pos->pieces[sq-9], list);
            }
            if(!SQOFFBOARD(sq-11) && PieceCol[pos->pieces[sq-11]] == WHITE){
                AddBlackPawnCaptureMove(pos, sq, sq-11, pos->pieces[sq-11], list);
            }
            //check en passant capture if one is available
            if(pos->enPassant != NO_SQ){
                //generate en passant moves for black
                if(sq-9 == pos->enPassant){
                    AddCaptureMove(pos, MOVE(sq, sq-9, EMPTY, EMPTY, MFLAGEP), list);
                }
                if(sq-11 == pos->enPassant){
                    AddCaptureMove(pos, MOVE(sq, sq-11, EMPTY, EMPTY, MFLAGEP), list);
                }
            }
        }
    }
}