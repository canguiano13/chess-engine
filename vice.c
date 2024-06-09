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

int main(){
    //initialzes game with starting values
    AllInit();
    
    S_BOARD board[1];
    //print starting position
    ParseFen(START_FEN, board);
    PrintBoard(board);
    //print board after 1.e4
    ParseFen(FEN2, board);
    PrintBoard(board);
    //print board after 1..c5
    ParseFen(FEN3, board);
    PrintBoard(board);
    
    return 0;
}