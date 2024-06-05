#include "defs.h"

//function will initialize values when the engine starts running
int Sq120ToSq64[BRD_SQ_NUM]; 
int Sq64ToSq120[64];

U64 SetMask[64];
U64 ClearMask[64];

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
        ClearMask[index] |= ~SetMask[index];
    }
}
//initialize full-board representation
void InitSq120to64(){
    int index;
    int file;
    int rank;
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
    InitSq120to64();
    InitBitMasks();
}