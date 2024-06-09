//file to generate unique position IDs to individual chess positions in order to detect 3x repetition
//to generate the key, we'll:
    //1. generate random values for each piece on the board
    //2. find the XOR of all random values
#include "defs.h"

//generate a unique position ID based on current board position
U64 GeneratePosKey(const S_BOARD *pos){
    int sq = 0;
    U64 finalKey = 0ULL;
    int piece = EMPTY;

    //generate hash based on pieces on board
    for(sq=0; sq < BRD_SQ_NUM; sq++){
        piece = pos->pieces[sq];
        if(piece!=NO_SQ && piece != EMPTY){
            ASSERT(piece >= wP && piece <= bK);
            finalKey ^= PieceKeys[piece][sq];
        }
    }
    //modify key when it is white's turn to move
    if(pos -> side == WHITE){
        finalKey ^= SideKey;
    }
    //modify key when en passant is possible
    if(pos->enPassant != NO_SQ){
        //validate that en passant square is within full-board indices
        ASSERT(pos->enPassant>=0 && pos->enPassant < BRD_SQ_NUM);
        finalKey ^= PieceKeys[EMPTY][pos->enPassant];
    }
    //modify key based on castling permissions
    ASSERT(pos->enPassant >= 0 && pos->castlePerm<=15);
    finalKey ^= CastleKeys[pos->castlePerm];
    
    return finalKey;
}