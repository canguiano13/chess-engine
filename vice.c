#include <stdio.h>
#include <stdlib.h>
#include "attack.c"
#include "bitboards.c"
#include "board.c"
#include "data.c"
#include "defs.h"
#include "hashkeys.c"
#include "init.c"


//Temporary FEN positions to demonstrate printboard
#define FEN1 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"         //starting position
#define FEN2 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"      //1.e4
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"    //1.e4 c5
#define TEMP_FEN "5rk1/pp4pp/4p3/2R3Q1/3n4/2q4r/P1P2PPP/5RK1 b - - 0 1"         //Marshall's Gold Coins game before ..Qg3!!
#define TEMP_FEN2 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1" //FEN to test asserts
#define TEMP_FEN3 "8/3q1p2/8/5P2/4Q3/8/8/8 w - - 0 1" //FEN of illegal positino to test SqAttacked()

void ShowSqAtBySide(const int side, const S_BOARD *pos){
    int rank = 0;
    int file = 0;
    int sq = 0;

    printf("\n\nSquares attacked by: %c\n", SideChar[side]);
    for(rank =RANK_8; rank >= RANK_1; rank--){
        for(file = FILE_A; file <= FILE_H; file++){
            sq = FR2SQ(file, rank);
            if(SqAttacked(sq, side, pos) == TRUE){
                printf("X");
            }
            else{
                printf("-");
            }
        }
        printf("\n");
    }
}

int main(){
    //initialzes game with starting values
    AllInit();
    
    S_BOARD board[1];

    ParseFen(TEMP_FEN3, board);
    PrintBoard(board);

    printf("\n\nWhite Attacking:");
    ShowSqAtBySide(WHITE, board);

    printf("\n\nBlack Attacking:");
    ShowSqAtBySide(BLACK, board);

    return 0;
}