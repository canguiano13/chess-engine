#include <stdio.h>
#include "defs.h"
#include "init.c"

int main(){
    //initialzes game with starting values
    AllInit();
    
    int index;

    //TODO: remove print statements
    //print current board representations
    for(index = 0; index < BRD_SQ_NUM; index++){
        if(index%10==0) printf("\n");
        printf("%5d", Sq120ToSq64[index]);
    }

    printf("\n\n");

    for(index = 0; index < 64; index++){
        if(index%8==0) printf("\n");
        printf("%5d", Sq64ToSq120[index]);
    }

    return 0;
}