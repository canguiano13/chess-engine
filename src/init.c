#include "defs.h"
#include <stdlib.h>

//function will initialize values when the engine starts running
int Sq120ToSq64[BRD_SQ_NUM]; 
int Sq64ToSq120[64];

U64 SetMask[64];
U64 ClearMask[64];

//arrays used later to generate position keys and determine castling permissions
U64 PieceKeys[13][120];
U64 CastleKeys[16];
//use to determine who's side is to move
U64 SideKey;

int FilesBrd[BRD_SQ_NUM];
int RanksBrd[BRD_SQ_NUM];

//bit masks for detecting passed and isolani pawns
U64 FileBBMask[8];
U64 RankBBMask[8];

U64 BlackPassedMask[64];
U64 WhitePassedMask[64];
U64 IsolatedMask[64];

void InitEvalMasks(){
    int sq, tsq, r, f;

    for(sq = 0; sq < 8; sq++){
        FileBBMask[sq] = 0ULL;
        RankBBMask[sq] = 0ULL;
    }

    for(r = RANK_8; r >= RANK_1; r--){
        for(f = FILE_A; f <= FILE_H; f++){
            sq = r*8 + f;
            FileBBMask[f] |= (1ULL << sq);
            RankBBMask[r] |= (1ULL << sq);
        }
    }

    for(sq=0; sq<64; sq++){
        IsolatedMask[sq] = 0ULL;
        WhitePassedMask[sq] = 0ULL;
        BlackPassedMask[sq] = 0ULL;
    }

    for(sq = 0; sq < 64; sq++){
        tsq = sq + 8;
        while(tsq < 64){
            WhitePassedMask[sq] |= (1ULL << tsq);
            tsq+=8;
        }

        tsq = sq-8;
        while(tsq >= 0){
            BlackPassedMask[sq] |= (1ULL << tsq);
            tsq-=8;
        }
        //locate any passed pawns on the board using the bit masks
        if(FilesBrd[SQ120(sq)] > FILE_A){
            IsolatedMask[sq] |= FileBBMask[FilesBrd[SQ120(sq)] - 1];
            
            tsq = sq+7;
            while(tsq<64){
                WhitePassedMask[sq] |= (1ULL << tsq);
                tsq+=8;
            }
            tsq = sq-9;
            while(tsq >= 0){
                BlackPassedMask[sq] |= (1ULL << tsq);
                tsq-=8;
            }
        }

        if(FilesBrd[SQ120(sq)] < FILE_H){
            IsolatedMask[sq] |= FileBBMask[FilesBrd[SQ120(sq)] + 1];
            
            tsq = sq+9;
            while(tsq<64){
                WhitePassedMask[sq] |= (1ULL << tsq);
                tsq+=8;
            }
            tsq = sq-7;
            while(tsq >= 0){
                BlackPassedMask[sq] |= (1ULL << tsq);
                tsq-=8;
            }
        }
    }

}

void InitFilesRanksBrd(){
    int i = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;

    for(i = 0; i<BRD_SQ_NUM; i++){
        FilesBrd[i] = OFFBOARD;
        RanksBrd[i] = OFFBOARD;
    }

    for(rank = RANK_1; rank <= RANK_8; rank++){
        for(file = FILE_A; file <= FILE_H; file++){
            sq = FR2SQ(file, rank);
            FilesBrd[sq] = file;
            RanksBrd[sq] = rank;
        }
    }
}

void InitHashKeys(){
    int i, j;
    for(i = 0; i < 13; i++){
        for(j = 0; j < 120; j++){
            PieceKeys[i][j] = RAND_64;
        }
    }

    SideKey = RAND_64;

    for(i = 0; i<16; i++){
        CastleKeys[i] = RAND_64;
    }
}
//set bit masks for setting/clearing individual board bits
void InitBitMasks(){
    int index = 0;
    //initialize two masks that will be used to set individual bits on the board with all zeroes
    for(index = 0; index < 64; index++){
        SetMask[index] = 0ULL;
        ClearMask[index] = 0ULL;
    }

    for(index = 0; index < 64; index++){
        SetMask[index] |= (1ULL << index);
        //clearmask values to complement of setmask
        ClearMask[index] = ~SetMask[index];
    }
}
//initialize full-board representation
void InitSq120to64(){
    int index=0;
    int file=FILE_A;
    int rank=RANK_1;
    int sq = A1;
    int sq64 = 0;

    //iterate through all squares in 64-board representation and set value to sentinel
    for(index = 0; index < BRD_SQ_NUM; index++){
        Sq120ToSq64[index] = 65;
    }
    //iterator over all squares in full-board representation and set value to sentinel
    for(index = 0; index < 64; index++){
        Sq64ToSq120[index] = 120;
    }

    //iterate over 64-board squares
    for(rank = RANK_1; rank <= RANK_8; rank++){
        for(file = FILE_A; file <= FILE_H; file++){
            //determine full-board index from file and rank
            sq = FR2SQ(file, rank);
            //set full-board index
            Sq64ToSq120[sq64] = sq;
            //set 64-board index
            Sq120ToSq64[sq] = sq64;
            sq64++;
        }
    }
}

void AllInit(){
    printf("Initializing VICE...\n");
    InitSq120to64();
    InitBitMasks();
    InitHashKeys();
    InitFilesRanksBrd();
    InitEvalMasks();
    InitMvvLva();
}