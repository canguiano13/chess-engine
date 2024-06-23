#include "defs.h"

#define HASH_PCE(pce, sq) (pos->posKey ^= (PieceKeys[(pce)][(sq)]))
#define HASH_CA (pos->posKey ^= (CastleKeys[(pos->castlePerm)]))
#define HASH_SIDE (pos->posKey ^= SideKey)
#define HASH_EP (pos->posKey ^= (PieceKeys[EMPTY][(pos->enPassant)]))

//modify castling permissions when king moves from a given square
const int CastlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
};

static void ClearPiece(const int sq, S_BOARD *pos){
    //validate square index
    ASSERT(SqOnBoard(sq));

    //validate piece being removed from square
    int pce = pos->pieces[sq];
    ASSERT(PieceValid(pce));

    int col = PieceCol[pce];
    int index = 0;
    int t_pceNum = -1;

    //validate active side
    ASSERT(SideValid(col));

    //hash piece from position key
    HASH_PCE(pce, sq);

    //change square's state and remove material from side
    pos->pieces[sq] = EMPTY;
    pos->material[col] -= PieceVal[pce];

    //remove piece from piece trackers
    if(PieceBig[pce]){
        pos->bigPce[col]--;
        if(PieceMaj[pce]){
            pos->majPce[col]--;
        }
        else{
            pos->minPce[col]--;
        }
    }//else piece is a pawn
    else{
        CLRBIT(pos->pawns[col], SQ64(sq));
        CLRBIT(pos->pawns[BOTH], SQ64(sq));
    }

    //remove piece from piece list
    for(index = 0; index < pos->pceNum[pce]; index++){
        if(pos->pList[pce][index] == sq){
            t_pceNum = index;
            break;
        }
    }

    //validate that square was found in pList
    ASSERT(t_pceNum != -1);
    ASSERT(t_pceNum >= 0 && t_pceNum < 10);

    pos->pceNum[pce]--;
    pos->pList[pce][t_pceNum] = pos->pList[pce][pos->pceNum[pce]];
}

static void AddPiece(const int sq, S_BOARD *pos, const int pce){
    //validate piece and square
    ASSERT(PieceValid(pce));
    ASSERT(SqOnBoard(pce));

    //store color of piece being added
    int col = PieceCol[pce];
    //validate active side
    ASSERT(SideValid(col));

    //hash pce into key
    HASH_PCE(pce, sq);

    //add piece on board
    pos->pieces[sq] = pce;

    //update piece counters
    if(PieceBig[pce]){
        pos->bigPce[col]++;
        if(PieceMaj[pce]){
            pos->majPce[col]++;
        }
        else{
            pos->minPce[col]++;
        }
    }//else adding a pawn
    else{
        SETBIT(pos->pawns[col], SQ64(sq));
        SETBIT(pos->pawns[BOTH], SQ64(sq));
    }

    //add material to side's total material count
    pos->material[col] += PieceVal[pce];
    //add piece's square to piecelist
    pos->pList[pce][pos->pceNum[pce]++] = sq;

}   

static void MovePiece(const int from, const int to, S_BOARD *pos){
    //validate starting and destination squares
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));

    int index = 0;
    int pce = pos->pieces[from];
    int col = PieceCol[pce];

#ifdef DEBUG
        int t_PieceNum = FALSE;
#endif

    //hash piece's current sq out of position key
    HASH_PCE(pce, from);
    //remove pce from board representation
    pos->pieces[from] = EMPTY;

    //hash piece's new sq into postion key
    HASH_PCE(pce, to);
    pos->pieces[to] = pce;

    if(!PieceBig[pce]){
        //clear pawn from preivous bitboard position
        CLRBIT(pos->pawns[col], SQ64(from));
        CLRBIT(pos->pawns[BOTH], SQ64(from));

        //add pawn with new bitboard position
        SETBIT(pos->pawns[col], SQ64(to));
        SETBIT(pos->pawns[BOTH], SQ64(to));
    }

    for(index = 0; index < pos->pceNum[pce]; index++){
        if(pos->pList[pce][index] == from){
            pos->pList[pce][index] = to;
#ifdef DEBUG
    t_PieceNum = TRUE;
#endif
        break;
        }
    } 
    ASSERT(t_PieceNum);
}

int MakeMove(S_BOARD *pos, int move){
    //validate current position
    ASSERT(CheckBoard(pos));

    //determine start and destination squares, active side
    int from = FROMSQ(move);
    int to = TOSQ(move);
    int side = pos->side;

    //validate start and destination squares, active side  
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(SideValid(side));
    ASSERT(PieceValid(pos->pieces[from]));

    //store current positionID
    pos->history[pos->hisPly].posKey = pos->posKey;

    //check for en passant capture
    if(move & MFLAGEP){
        if(side == WHITE){
            ClearPiece(to-10, pos);
        }
        else{
            ClearPiece(to+10, pos);
        }
    }
    //check for castle
    else if(move & MFLAGCA){
        switch(to){
            //WQCA
            case C1:
                MovePiece(A1, D1, pos);
                break;
            //BQCA
            case C8:
                MovePiece(A8, D8, pos);
                break;
            //WKCA
            case G1:
                MovePiece(H1, F1, pos);
                break;
            //BKCA
            case G8:
                MovePiece(H8, F8, pos);
                break;
            default: ASSERT(FALSE); break;
        }
    }

    //hash out en-passant/castle from positionID
    if(pos->enPassant != NO_SQ) HASH_EP;
    HASH_CA;

    //store current state of game
    pos->history[pos->hisPly].move = move;
    pos->history[pos->hisPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->hisPly].enPassant = pos->enPassant;
    pos->history[pos->hisPly].castlePerm = pos->castlePerm;

    //Update castle permissions / en passant
    pos->castlePerm &= CastlePerm[from];
    pos->castlePerm &= CastlePerm[to];
    pos->enPassant = NO_SQ;

    HASH_CA;

    //update fifty move rule
    pos->fiftyMove++;

    //reset fifty move rule if piece was captured
    int captured = CAPTURED(move);
    if(captured != EMPTY){
        ASSERT(PieceValid(captured));
        ClearPiece(to, pos);
        pos->fiftyMove = 0;
    }

    //update move counter
    pos->hisPly++;
    pos->ply++;

    //check for new en passant square
    if(PiecePawn[pos->pieces[from]]){
        pos->fiftyMove = 0;
        if(move & MFLAGPS){
            if(side == WHITE){
                pos->enPassant = from + 10;
                ASSERT(RanksBrd[pos->enPassant] == RANK_3);
            }
            else{
                pos->enPassant = from - 10;
                ASSERT(RanksBrd[pos->enPassant] == RANK_6);
            }
            HASH_EP;
        }
    }

    MovePiece(from, to, pos);

    //check for promotion
    int prPce = PROMOTED(move);
    if(prPce != EMPTY){
        ASSERT(PieceValid(prPce) && !PiecePawn[prPce]);
        ClearPiece(to, pos);
        AddPiece(to, pos, prPce);
    }

    //update king square if king moved
    if(PieceKing[pos->pieces[to]]){
        pos->KingSQ[pos->side] = to;
    }

    //update active side
    pos->side ^= 1;
    HASH_SIDE;

    ASSERT(CheckBoard(pos));

    //If previous side was in check, takeback move
    if(SqAttacked(pos->KingSQ[side], pos->side, pos)){
        TakeMove(pos);
        return FALSE;
    }

    return TRUE;
}

//take back a move from the current position
void TakeMove(S_BOARD *pos){
    //validate board
    ASSERT(CheckBoard(pos));

    //decrease move trackers
    pos->hisPly--;
    pos->ply--;

    int move = pos->history[pos->hisPly].move;
    int from = FROMSQ(move);
    int to = TOSQ(move);

    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));

    if(pos->enPassant != NO_SQ){
        HASH_EP;
    }
    HASH_CA;

    //revert game state
    pos->castlePerm = pos->history[pos->hisPly].castlePerm;
    pos->fiftyMove = pos->history[pos->hisPly].fiftyMove;
    pos->enPassant = pos->history[pos->hisPly].enPassant;

    if(pos->enPassant != NO_SQ){
        HASH_EP;
    }
    HASH_CA;

    //change active side
    pos->side ^= 1;
    HASH_SIDE;

    //reverse enPassant move
    if(MFLAGEP & move){
        if(pos->side == WHITE){
            AddPiece(to-10, pos, bP);
        }
        else{
            AddPiece(to+10, pos, wP);
        }
    }
    //reverse castling move
    else if(MFLAGCA & move){
        switch(to){
            case C1: MovePiece(D1, A1, pos); break;
            case C8: MovePiece(D8, A8, pos); break;
            case G1: MovePiece(F1, H1, pos); break;
            case G8: MovePiece(F8, H8, pos); break;
            default: ASSERT(FALSE); break;
        }
    }

    MovePiece(to, from, pos);

    //undo king move
    if(PieceKing[pos->pieces[from]]){
        pos->KingSQ[pos->side] = from;
    }

    //undo capture move
    int captured = CAPTURED(move);
    if(captured != EMPTY){
        ASSERT(PieceValid(captured));
        AddPiece(to, pos, captured);
    }

    //undo promotion
    if(PROMOTED(move) != EMPTY){
        ASSERT(PieceValid(PROMOTED(move)) && !PiecePawn[PROMOTED(move)]);
        ClearPiece(from, pos);
        AddPiece(from, pos, (PieceCol[PROMOTED(move)] == WHITE ? wP:bP));
    }

    ASSERT(CheckBoard(pos));
}


