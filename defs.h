//include guard to guard against multiple includes when linking
#ifndef DEFS_H
#define DFS_H

//unsigned 64 bit integer used in the program
typedef unsigned long long U64;

//define the name and number of squares considered by the board
#define NAME "Vice 1.0"
#define BRD_SQ_NUM 120

//enumerate the names of the pieces
enum {EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK};
//enumerate the ranks and files for the board
enum {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE};
enum {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE};
//enumerate the colors for each side
enum {WHITE, BLACK, BOTH};
//enumerate the squares of the board
enum{
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ //off-board square has value 99
};
//adds true/false
enum{FALSE, TRUE};

//define main board structure
typedef struct {
    //define array to track the piece currently on each square
    //squares with no piece will have EMPTY, enumerated above
    int pieces[BRD_SQ_NUM];

    //determine if each square has a white/black pawn, or no pawn
    //for example pawns[0]=00000010 00000000 00000000 00000000 00000000 00000000 00000000 00000000 defines a white pawn on the first row, second to last square
    //pawns[2] represents the pawns for both sides
    U64 pawns[3];

    int KingSQ[2];  //track squares on which the kings are placed
    int side;       //track of whose turn it is
    int enPass;     //track of en passant square
    int fifty;      //detect fifty-move rule
    int ply;        //track ply
    int hisPly;     //track total number of moves made over course of the game
    U64 posKey;     //unique position id to track 3-fold repetition

    //create piece trackers to evaluate positions
    int pceNum[13]; //track the number of pieces on the board for each piece type
    int notPawn[3]; //track the number of non-pawn pieces for each side
    int majPce[3];  //track the number of major pieces for each side
    int minPce[3];  //track the number of minor pieces for each side
} S_BOARD;

#endif