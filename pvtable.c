#include <stdio.h>
#include "defs.h"

const int HashSize = 0x100000 * 64;

int GetPvLine(const int depth, S_BOARD *pos){
    ASSERT(depth < MAX_SEARCH_DEPTH);

    int move = ProbePvMove(pos);
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
        move = ProbePvMove(pos);
    }

    //take back all moves made in the search
    while(pos->ply > 0){
        TakeMove(pos);
    }

    return count;
}

void ClearHashTable(S_HASHTABLE *table){
    S_HASHENTRY *tableEntry;

    for(tableEntry = table->pTable; tableEntry < (table->pTable + table->numEntries); tableEntry++){
        tableEntry->posKey = 0ULL;
        tableEntry->move = NOMOVE;
        tableEntry->depth = 0;
        tableEntry->score = 0;
        tableEntry->flags = 0;
    }
    table->newWrite = 0;
}

void InitHashTable(S_HASHTABLE *table){
    table->numEntries = HashSize / sizeof(S_HASHENTRY);
    table->numEntries -= 2;
    
    //clear memory for table data if memory is not freed already
    // table->pTable = 0;
    if(table->pTable != NULL) {
        free(table->pTable);
    }
    //allocate space for hash table entries
    table->pTable = (S_HASHENTRY*) malloc(table->numEntries * sizeof(S_HASHENTRY));

    if(table->pTable) printf("Hash Table Allocation Complete.\n");

    ClearHashTable(table);
}

int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth){
    int index = pos->posKey % pos->HashTable->numEntries;

    ASSERT(index >= 0 && index <= (pos->HashTable->numEntries-1));
    ASSERT(depth >= 1 && depth < MAX_SEARCH_DEPTH)
    ASSERT(alpha < beta);
    ASSERT(alpha >= -INFINITY && alpha <= INFINITY);
    ASSERT(beta >= -INFINITY && beta <= INFINITY);
    ASSERT(pos->ply >= 0 && pos->ply < MAX_SEARCH_DEPTH);

    if(pos->HashTable->pTable[index].posKey == pos->posKey){
        *move = pos->HashTable->pTable[index].move;
        if(pos->HashTable->pTable[index].depth >= depth){
            pos->HashTable->hit++;

            ASSERT(pos->HashTable->pTable[index].depth >= 1 && pos->HashTable->pTable[index].depth < MAX_SEARCH_DEPTH);
            ASSERT(pos->HashTable->pTable[index].flags >= HFALPHA && pos->HashTable->pTable[index].flags <= HFEXACT);

            *score = pos->HashTable->pTable[index].score;

            if(*score > MATE) *score -= pos->ply;
            else if(*score < MATE) *score += pos->ply;

            switch(pos->HashTable->pTable[index].flags){
                case HFALPHA:
                    if(*score <= alpha){
                        *score = alpha;
                        return TRUE;
                    }
                    break;
                case HFBETA:
                    if(*score <= beta){
                        *score = beta;
                        return TRUE;
                    }
                    break;
                case HFEXACT:
                    return TRUE;
                    break;
                default: ASSERT(FALSE); break;
            }
        }
    }

    return FALSE;
}

void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth){
    //calculate hash table index based on positionID
    int index = pos->posKey % pos->HashTable->numEntries;

    //validate hash entry properties
    ASSERT(index >= 0 && index <= pos->HashTable->numEntries-1);
    ASSERT(depth >= 1 && depth < MAX_SEARCH_DEPTH);
    ASSERT(flags >= HFALPHA && flags <= HFEXACT);
    ASSERT(score >= -INFINITY && score <= INFINITY);
    ASSERT(pos->ply >= 0 && pos->ply < MAX_SEARCH_DEPTH);

    //handle collision
    if(pos->HashTable->pTable[index].posKey == 0){
        pos->HashTable->newWrite++;
    }
    else{
        pos->HashTable->overWrite++;
    }

    if(score > MATE) score+=pos->ply;
    else if(score < -MATE) score-= pos->ply;

    //insert hash table entry
    pos->HashTable->pTable[index].move = move;
    pos->HashTable->pTable[index].posKey = pos->posKey;
    pos->HashTable->pTable[index].flags = flags;
    pos->HashTable->pTable[index].score = score;
    pos->HashTable->pTable[index].depth = depth;
}

int ProbePvMove(const S_BOARD *pos){
    //get hash table index based on positionID
    int index = (pos->posKey) % (pos->HashTable->numEntries);
    //validate index based on hash table bounds
    ASSERT(index >= 0 && index <= pos->HashTable->numEntries-1);
    //return corresponing hash table entry if it's key exists in the table
    if(pos->HashTable->pTable[index].posKey == pos->posKey){
        return pos->HashTable->pTable[index].move;
    }    
    //if key does not exist, return empty move
    return NOMOVE;
}



