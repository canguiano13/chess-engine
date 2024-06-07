#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "init.c"
#include "bitboards.c"

int main(){
    //initialzes game with starting values
    AllInit();
    U64 playBitBoard = 0ULL;
    
    //show board after setting individual bit with bitmasks
    for(int index = 0; index < 64; index++){
        printf("Index: %d", index);
        PrintBitBoard(SetMask[index]);
        printf("\n");
    }
    //show board after clearing individual bits with bitmasks
    for(int index = 0; index < 64; index++){
        printf("Index: %d", index);
        PrintBitBoard(ClearMask[index]);
        printf("\n");
    }
    
    return 0;
}