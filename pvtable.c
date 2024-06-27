#include <stdio.h>
#include "defs.h"

const int PvSize = 0x100000 * 2;

int GetPvLine(const int depth, S_BOARD *pos){
    ASSERT(depth < MAX_SEARCH_DEPTH);

    int move = ProbePvTable(pos);
    int count = 0;

    while(move != NOMOVE && count < depth){
        ASSERT(count < MAX_SEARCH_DEPTH);
        
        if(MoveExists(pos, move)){
            MakeMove(pos, move);
            pos->PvArray[count++] = move;
        }
        else{
            break;
        }
        move = ProbePvTable(pos);
    }

    //take back all moves made in the search
    while(pos->ply > 0){
        TakeMove(pos);
    }

    return count;
}

void ClearPvTable(S_PVTABLE *table){
    S_PVENTRY *pvEntry;

    for(pvEntry = table->pTable; pvEntry < (table->pTable + table->numEntries); pvEntry++){
        pvEntry->posKey = 0ULL;
        pvEntry->move = NOMOVE;
    }
}

void InitPvTable(S_PVTABLE *table){
    table->numEntries = PvSize / sizeof(S_PVENTRY);
    table->numEntries -= 2;
    
    //clear memory for table data if memory is not freed already
    table->pTable = 0;
    if(table->pTable != NULL) {
        free(table->pTable);
    }
    //allocate space for hash table entries
    table->pTable = (S_PVENTRY *) malloc(table->numEntries * sizeof(S_PVENTRY));

    ClearPvTable(table);
    printf("PvTable init complete with %d entries\n", table->numEntries);
}

void StorePvMove(const S_BOARD *pos, const int move){
    /* *TODO handle hash table collisions */

    //get hash table index based on positionID
    int index = (pos->posKey) % (pos->PvTable->numEntries);
    //validate index based on hash table bounds
    ASSERT(index >= 0 && index <= pos->PvTable->numEntries-1);
    //insert hash table entry
    pos->PvTable->pTable[index].move = move;
    pos->PvTable->pTable[index].posKey = pos->posKey;
}

int ProbePvTable(const S_BOARD *pos){
    //get hash table index based on positionID
    int index = (pos->posKey) % (pos->PvTable->numEntries);
    //validate index based on hash table bounds
    ASSERT(index >= 0 && index <= pos->PvTable->numEntries-1);
    //return corresponing hash table entry if it's key exists in the table
    if(pos->PvTable->pTable[index].posKey == pos->posKey){
        return pos->PvTable->pTable[index].move;
    }    
    //if key does not exist, return empty move
    return NOMOVE;
}



