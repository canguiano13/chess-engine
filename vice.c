#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "init.c"
#include "bitboards.c"
#include "board.c"
#include "hashkeys.c"
#include "data.c"

//Temporary FEN positions to demonstrate printboard
#define FEN1 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"         //starting position
#define FEN2 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"      //1.e4
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"    //1.e4 c5
#define TEMP_FEN "5rk1/pp4pp/4p3/2R3Q1/3n4/2q4r/P1P2PPP/5RK1 b - - 0 1"         //Marshall's Gold Coins game before ..Qg3!!
#define TEMP_FEN2 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1" //FEN to test asserts
int main(){
    //initialzes game with starting values
    AllInit();
    
    S_BOARD board[1];

    //test FEN 1
    ParseFen(TEMP_FEN, board);
    PrintBoard(board);
    printf("\nwp: \n");
    PrintBitBoard(board->pawns[WHITE]);
    printf("\nbP: \n");
    PrintBitBoard(board->pawns[BLACK]);
    printf("\nboth: \n");
    PrintBitBoard(board->pawns[BOTH]);
    ASSERT(CheckBoard(board));

    //test FEN 2
    ParseFen(TEMP_FEN2, board);
    PrintBoard(board);
    printf("\nwp: \n");
    PrintBitBoard(board->pawns[WHITE]);
    printf("\nbP: \n");
    PrintBitBoard(board->pawns[BLACK]);
    printf("\nboth: \n");
    PrintBitBoard(board->pawns[BOTH]);
    ASSERT(CheckBoard(board));
    
    return 0;
}