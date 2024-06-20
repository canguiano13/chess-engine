#include <stdio.h>
#include <stdlib.h>
#include "attack.c"
#include "bitboards.c"
#include "board.c"
#include "data.c"
#include "defs.h"
#include "hashkeys.c"
#include "init.c"
#include "io.c"
#include "makemove.c"
#include "movegen.c"
#include "validate.c"


//Temporary FEN positions to demonstrate methods
#define FEN1 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"         //starting position
#define FEN2 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"      //1.e4
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"    //1.e4 c5

#define TEMP_FEN "5rk1/pp4pp/4p3/2R3Q1/3n4/2q4r/P1P2PPP/5RK1 b - - 0 1"         //Marshall's Gold Coins game before ..Qg3!!
#define TEMP_FEN2 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1" //FEN to test asserts
#define TEMP_FEN3 "8/3q1p2/8/5P2/4Q3/8/8/8 w - - 0 1" //FEN to test SqAttacked()

#define WHITE_PAWN_MOVE_GEN_TEST "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1" //FEN tests move generation for white pawns
#define BLACK_PAWN_MOVE_GEN_TEST "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1" //FEN tests move generation for black pawns

#define KNIGHT_KING_MOVE_GEN_TEST "5k2/1n6/4n3/6N1/8/3N4/8/5K2 b - - 0 1"
#define ROOK_MOVE_GEN_TEST "6k1/8/5r2/8/1nR5/5N2/8/6K1 w - - 0 1"
#define QUEEN_MOVE_GEN_TEST "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 w - - 0 1"
#define BISHOP_MOVE_GEN_TEST "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 w - - 0 1"

#define CASTLE_MOVE_GEN_TEST "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
#define NO_CASTLE_MOVE_GEN_TEST "3rk2r/8/8/8/8/8/6p1/R3K2R w KQk - 0 1"

int main(){
    //initialzes game with starting values
    AllInit();
    
    S_BOARD board[1];

    ParseFen(TEMP_FEN2, board);
    PrintBoard(board);
    printf("\n");
    S_MOVELIST list[1];
    GenerateAllMoves(board, list);
    PrintMoveList(list);

    return 0;
}