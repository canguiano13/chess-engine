//include guard to guard against multiple includes when linking
#ifndef DEFS_H
#define DEFS_H

#include "stdlib.h"
#include <stdio.h>

//#define DEBUG

//debug macro
#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s - Failed ", #n); \
printf("on %s", __DATE__); \
printf(" at %s", __TIME__); \
printf(" in file: %s ", __FILE__); \
printf("at Line %d ", __LINE__); \
exit(1);}
#endif

//unsigned 64 bit integer used in the program
typedef unsigned long long U64;

//define the name and number of squares considered by the board
#define NAME "Vice 1.0"
#define BRD_SQ_NUM 120
#define MAX_GAME_MOVES 2048 //defines the max number of moves (in ply) that will be considered for a game
#define MAX_POS_MOVES 256 //maximum possible number of available moves in any single position
#define MAX_SEARCH_DEPTH 64

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

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
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD //off-board square has value 99
};
//add true/false
enum{FALSE, TRUE};

//castling permissions for kingside/queenside for each side
//uses 4 bits which hold 1/0 for allowed/not allowed to castle to corresponding side
enum {WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8};

//structure for individual game moves
typedef struct{
    int move;  //stores information about actual move
    int score; //helps move search tree
} S_MOVE;
typedef struct{
    S_MOVE moves[MAX_POS_MOVES];
    int count;
} S_MOVELIST;
//structure for hash table
typedef struct{
    U64 posKey;
    int move;
} S_PVENTRY;
typedef struct{
    S_PVENTRY *pTable;
    int numEntries;
} S_PVTABLE;

//store history of game properties
typedef struct {
    int move;       //previous move
    int castlePerm; //castling permissions from previous move(s)
    int enPassant;     //en passant permissions from previous move(s)
    int fiftyMove;  //fifty move rule status from previous move(s)
    U64 posKey;     //3x repetition status from previous move(s)
} S_UNDO;

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
    int enPassant;  //track of en passant square
    int fiftyMove;  //detect fifty-move rule
    int ply;        //track ply
    int hisPly;     //track history of ply (used to index history array)
    int castlePerm; //track castling permissions for each side
    U64 posKey;     //unique position id to track 3-fold repetition

    //create piece trackers to evaluate positions
    int pceNum[13]; //track the number of pieces on the board for each piece type
    int bigPce[2];  //track the number of non-pawn pieces for each side
    int majPce[2];  //track the number of major pieces for each side
    int minPce[2];  //track the number of minor pieces for each side
    int material[2]; //track the total amount of material for each side

    S_UNDO history[MAX_GAME_MOVES]; //track history of the game in case we want to undo a move

    //maintain piece list
    //set maximum number of pieces on the board to 10
    //pList[wN][0] = E1 adds a knight to E1
    //pList[wN][1] = F1 adds a second, separate knight to F1
    int pList[13][10];

    //principal variation table
    S_PVTABLE PvTable[1];
    //table will ideally hold the best line (variation)
    int PvArray[MAX_SEARCH_DEPTH];

    //heuristic used when alpha score is improved upon during search to identify piece squares that contribute to better alpha scores
    int searchHistory[13][BRD_SQ_NUM];
    //store most recent non-capture moves used for beta cutoff
    int searchKillers[2][MAX_SEARCH_DEPTH];
} S_BOARD;


typedef struct{
    int startTime;
    int stoptTime;
    int timeSet;    //overall time limit for search

    int depth;      //current search depth
    int depthSet;   //overall search depth limit

    int quit;       //boolean which interrupts program
    int stopped;    //boolean which halts search, but not program

    int movesToGo; //moves left to search
    int infinite;

    long nodes;     //count of all positions searched in search tree
} S_SEARCHINFO;

/* GAME MOVE */
/*
    Using bits, we store information about a given move:
    7 bits - SqFrom
    7 bits - SqTo
    4 bits - Captured
    1 bit - EnPassant?
    1 bit - Pawn start
    4 bits - Piece Promotion?
    1 bit - Castle?
*/
#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) ((m>>7) & 0x7F)
#define CAPTURED(m) ((m>>14) & 0xF)
#define PROMOTED(m) ((m>>20) & 0xF)

#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000
#define MFLAGCAP 0x7c000
#define MFLAGPROM 0xF00000

#define NOMOVE 0

#define INFINITE 30000
#define ISMATE (INFINITE - MAX_SEARCH_DEPTH)

/* MACROS */
//use to determine full-board index from file and rank
//e.g. A1 = (A, 1) -> 21
#define FR2SQ(f, r) ( (21 + (f) ) + ( (r) * (10) ))
//shorthand to determine 64-board index from full-board index
#define SQ64(sq120) (Sq120ToSq64[(sq120)])
//shorthand to determine full-board index from 64-board index
#define SQ120(sq64) (Sq64ToSq120[(sq64)])
//shorthand macro for PopBit
#define POP(b) PopBit(b)
//shorthand macro for CountBits
#define CNT(b) CountBits(b)

//macros for bitmasks
#define CLRBIT(bb, sq) ((bb) &= ClearMask[(sq)])
#define SETBIT(bb, sq) ((bb) |= SetMask[(sq)])

//macros for piece validators
#define IsBQ(p) (PieceBishopQueen[(p)])
#define IsRQ(p) (PieceRookQueen[(p)])
#define IsKN(p) (PieceKnight[(p)])
#define IsKI(p) (PieceKing[(p)])

//retrieves corresponding square on black side for a given board square (eg. c3->c6, a8->a1)
#define MIRROR64(sq) (Mirror64[(sq)])

//fill 64 bits with random numbers (for position key)
#define RAND_64 (       (U64) rand() | \
                        (U64) rand() << 15 | \
                        (U64) rand() << 30 | \
                        (U64) rand() << 45 | \
                        ((U64) rand() & 0xf) << 60      ) //generates last 4/64 bits

/* GLOBALS */
//variables accessed everywhere use extern keyword
//these allow us to convert between full-board and 64-square indices
//not possible without these due to index mismatch in board representation
extern int Sq120ToSq64[BRD_SQ_NUM]; //use to determine 64-square index from full-board index including out-of-bounds squares
extern int Sq64ToSq120[64]; //use to determine full-board index based on 64-square index

//use to set and clear single bits on the bit board
extern U64 SetMask[64];
extern U64 ClearMask[64];

//arrays used later to generate position keys
extern U64 PieceKeys[13][120];
extern U64 CastleKeys[16];
extern U64 SideKey;

//use arrays to display pieces on board
extern char PceChar[];
extern char SideChar[];
extern char RankChar[];
extern char FileChar[];

//use arrays to track each type of piece for both sides
extern int PieceBig[13];
extern int PieceMaj[13];
extern int PieceMin[13];
extern int PieceVal[13];
extern int PieceCol[13];

//use arrays to determine rank/file for a given square
extern int FilesBrd[BRD_SQ_NUM];
extern int RanksBrd[BRD_SQ_NUM];

//returns true for the corresponing piece, and false otherwise
extern int PiecePawn[13];
extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRookQueen[13];
extern int PieceBishopQueen[13];
extern int PieceSlides[13];

//mirrors 64-sq based index for black
extern int Mirror64[64];
/* FUNCTIONS */
//attack.c
extern int SqAttacked(const int sq, const int side, const S_BOARD *pos);

//bitboards.c
extern int PopBit(U64 *bb);
extern int CountBits(U64 b);
extern void PrintBitBoard(U64 bb);

//board.c
extern void ResetBoard(S_BOARD *pos);
extern int ParseFen(char *fen, S_BOARD *pos);
extern void PrintBoard(S_BOARD *pos);
extern void UpdateListsMaterial(S_BOARD *pos);
extern int CheckBoard(const S_BOARD *pos);

//evaluate.c
int EvalPosition(const S_BOARD *pos);

//hashkeys.c
extern U64 GeneratePosKey(const S_BOARD *pos);

//init.c
extern void AllInit();

//io.c
extern char *PrSq(const int sq);
extern char *PrMove(const int move);
extern int ParseMove(char *ptrChar, S_BOARD *pos);
extern void PrintMoveList(const S_MOVELIST *list);

//makemove.c
extern int MakeMove(S_BOARD *pos, int move);
extern void TakeMove(S_BOARD *pos);

//misc.c
extern int GetTimeMs();

//movegen.c
extern void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list);
extern int MoveExists(S_BOARD *pos, const int move);

//perft.c
extern void PerftTest(int depth, S_BOARD *pos);

//pvtable.c
extern void InitPvTable(S_PVTABLE *table);
extern void StorePvMove(const S_BOARD *pos, const int move);
extern int ProbePvTable(const S_BOARD *pos);

//search.c
extern void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);

//validate.c
extern int SqOnBoard(const int sq);
extern int SideValid(const int side);
extern int FileRankValid(const int fr);
extern int PieceValid(const int pce);
extern int PieceValidEmpty(const int pce);

#endif