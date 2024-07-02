#include <stdio.h>
#include <stdlib.h>
#include "attack.c"
#include "bitboards.c"
#include "board.c"
#include "data.c"
#include "defs.h"
#include "evaluate.c"
#include "hashkeys.c"
#include "init.c"
#include "io.c"
#include "makemove.c"
#include "misc.c"
#include "movegen.c"
#include "perft.c"
#include "pvtable.c"
#include "search.c"
#include "uci.c"
#include "validate.c"
#include "xboard.c"

//Temporary FEN positions to demonstrate methods
#define SEARCH_TEST "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define UCI_TEST "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

int main(){
    //initialzes game with starting values
    AllInit();
    S_BOARD pos[1];
    S_SEARCHINFO info[1];
    InitPvTable(pos->PvTable);

    printf("\nWelcome to VICE! \nType \"vice\" to enter console mode, or \"uci\" or \"xboard\" for corresponding protocols.\n");
    printf(" > ");

    char line[256];
    while(TRUE){
        memset(&line[0], 0, sizeof(line));
        fflush(stdout);
        if(!fgets(line, 256, stdin)){
            continue;
        }
        if(line[0] == '\n'){
            continue;
        }
        if(!strncmp(line, "uci", 3)){
            UCI_Loop(pos, info);
            if(info->quit == TRUE) break;
            continue;
        }
        else if(!strncmp(line, "xboard", 6)){
            XBoard_Loop(pos, info);
            if(info->quit == TRUE) break;
            continue;
        }
        else if(!strncmp(line, "vice", 4)){
            Console_Loop(pos, info);
            if(info->quit == TRUE) break;
            continue;
        }
        else if(!strncmp(line, "quit", 4)){
            printf("\nThanks for Playing!\n\n");
            break;
        }
    }
    free(pos->PvTable->pTable);
    return 0;
}