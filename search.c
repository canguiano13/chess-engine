#include <stdio.h>
#include "defs.h"

#define INFINITY 30000
#define MATE 29000

//check for timer end or interrupt
static void CheckUp(){
    return;
}

//detect repetition of position
static int IsRepetition(const S_BOARD *pos){
    int index;
    //starting from last non-repeatable position, search for repetition of moves
    for(index = (pos->hisPly - pos->fiftyMove); index < pos->hisPly-1; index++){
        ASSERT(index >= 0 && index < MAX_GAME_MOVES)
        if(pos->posKey == pos->history[index].posKey){
            return TRUE;
        }
    }
    return FALSE;
}

//prepare for new search
static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info){
    int index = 0;
    int index2 = 0;
    //clear search heursistic data
    for(index = 0; index < 13; index++){
        for(index2 = 0; index2 < BRD_SQ_NUM; index2++){
            pos->searchHistory[index][index2]=0;
        }
    }
    for(index = 0; index < 2; index++){
        for(index2 = 0; index2 < MAX_SEARCH_DEPTH; index2++){
            pos->searchKillers[index][index2]=0;
        }
    }

    //clear hash table data
    ClearPvTable(pos->PvTable);
    pos->ply = 0; //clear current search ply

    //clear search info
    info->startTime = GetTimeMs();
    info->stopped = 0;
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;
}

//perform iteration of alpha-beta search
static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull){
    //validate current position
    ASSERT(CheckBoard(pos));

    //if there are no more moves to evaluate, return evaluation of the position and update the number of visted nodes
    if(depth == 0){
        info->nodes++;
        return EvalPosition(pos);
    }
    info->nodes++;

    //check draw by 3x repetition or fifty move rule
    if((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply){
        return 0;
    }

    //prevent searching too deep into the position
    if(pos->ply > MAX_SEARCH_DEPTH-1){
        return EvalPosition(pos);
    }

    //create move list and generate all moves for current position
    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int MoveNum = 0;
    int Legal = 0;  //check for number of legal moves in order to detect checkmate/stalemate
    int OldAlpha = alpha; //in iterative deepening, checks if next depth found a better move than last iteration's alpha
    int BestMove = NOMOVE;
    int Score = -INFINITY;

    for(MoveNum = 0; MoveNum < list->count; MoveNum++){
        //if move cannot be made, continue to next move in list
        if( !MakeMove(pos, list->moves[MoveNum].move)){
            continue;
        }
        Legal++;
        //call algorithm with next depth assuming the current move was played
        Score = -AlphaBeta(-beta, -alpha, depth-1, pos, info, TRUE);
        TakeMove(pos);

        if(Score > alpha){
            //beta cutoff
            if(Score >= beta){
                //check if first fail high case
                if(Legal == 1) info->fhf++;
                info-> fh++;
                return beta;
            }
            //else better alpha score was found
            alpha = Score;
            BestMove = list->moves[MoveNum].move;
        }
    }

    //check for stalemate/checkmate
    if(Legal == 0){
        if(SqAttacked(pos->KingSQ[pos->side], pos->side^1, pos)){
            return -MATE + pos->ply;
        }
        //else position is a draw
        else{
            return 0;
        }
    }
    //if a better move has been found than the last depth, store it
    if(alpha != OldAlpha){
        StorePvMove(pos, BestMove);
    }

    return alpha;
}

//quiescent search helps alleviate horizon effect
static int Quiescence(int alpha, int beta, S_BOARD *POS, S_SEARCHINFO *info){
    return 0;
}

//perform iterative deepening alpha-beta search to find next move
void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info){
    int bestMove = NOMOVE;
    int bestScore = -INFINITY;
    int currDepth = 0;
    int pvMoves = 0;
    int pvNum = 0;
    //prepare for search
    ClearForSearch(pos, info);
    //perform iterative deepening search
    for(currDepth = 1; currDepth <= info->depth; currDepth++){
        //perform alpha-beta search on current depth with params (alpha, beta, depth, pos, info, doNull)
        bestScore = AlphaBeta(-INFINITY, INFINITY, currDepth, pos, info, TRUE);

        //TODO: add timer check

        //store best variation and best next move
        pvMoves = GetPvLine(currDepth, pos);
        bestMove = pos->PvArray[0];
        //print move data to screen
        printf("\nDepth: %d\t score: %d\t move: %s\t nodes: %ld ", currDepth, bestScore, PrMove(bestMove), info->nodes);
        pvMoves = GetPvLine(currDepth, pos);
        printf("pv ");

        for(pvNum = 0; pvNum < pvMoves; pvNum++){
            printf(" %s", PrMove(pos->PvArray[pvNum]));
        }
        printf("\n");
        if(info->fh > 0)
            printf("Ordering: %.2f\n", (info->fhf/info->fh));
    }
}