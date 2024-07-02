#include <stdio.h>
#include "defs.h"

#define INFINITY 30000
#define MATE 29000

//check for timer end or interrupt
static void CheckUp(S_SEARCHINFO *info){
    if(info->timeSet == TRUE && GetTimeMs()>info->stopTime){
        info->stopped = TRUE;
    }
    ReadInput(info);
}

//reorder candidate moves 
static void PickNextMove(int moveNum, S_MOVELIST *list){
    S_MOVE temp;
    int index = 0;
    int bestScore = 0;
    int bestNum = moveNum;
    //order moves based on scoring in order to optimize search order
    for(index = moveNum; index < list->count; index++){
        if(list->moves[index].score > bestScore){
            bestScore = list->moves[index].score;
            bestNum = index;
        }
    }
    temp = list->moves[moveNum];
    list->moves[moveNum] = list->moves[bestNum];
    list->moves[bestNum] = temp;
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
    info->stopped = 0;
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;
}

//quiescent search helps alleviate horizon effect
static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info){
    //validate board position
    ASSERT(CheckBoard(pos));
    //check time limit
    if((info->nodes & 2047) == 0){
        CheckUp(info);
    }
    //add to number of nodes visited
    info->nodes++;
    //check for draw
    if((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply){
        return 0;
    }
    //check if maximum search depth has been surpassed
    if(pos->ply > MAX_SEARCH_DEPTH-1){
        return EvalPosition(pos);
    }

    //evaluate position resulting after completing alpha-beta search
    int Score = EvalPosition(pos);

    ASSERT(Score > -INFINITY && Score < INFINITY);

    //check for beta cutoff
    if(Score >= beta){
        return beta;
    }
    //check for standing pat
    if(Score > alpha){
        alpha = Score;
    }

    S_MOVELIST list[1];
    GenerateAllCaptures(pos, list);
    int MoveNum = 0;
    int Legal = 0;  //check for number of legal moves in order to detect checkmate/stalemate
    int OldAlpha = alpha;
    Score = -INFINITY;
    int BestMove = NOMOVE;
    //probe the pv table to check if we are in the principal variation
    int PvMove = ProbePvTable(pos);

    for(MoveNum = 0; MoveNum < list->count; MoveNum++){
        PickNextMove(MoveNum, list);
        if( !MakeMove(pos, list->moves[MoveNum].move) ){
            continue;
        }
        Legal++;
        Score = -Quiescence(-beta, -alpha,  pos, info);
        TakeMove(pos);

        if(info->stopped == TRUE){
                    return 0;
        }
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

    if(alpha != OldAlpha){
        StorePvMove(pos, BestMove);
    }

    return alpha;
}

//perform iteration of alpha-beta search
static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull){
    //validate current position
    ASSERT(CheckBoard(pos));

    //if there are no more moves to evaluate, return evaluation of the position and update the number of visted nodes
    if(depth == 0){
        return Quiescence(alpha, beta, pos, info);
    }
    //check time limit
    if((info->nodes & 2047) == 0){
        CheckUp(info);
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

    //if king is in check extend search depth by 1
    int InCheck = SqAttacked(pos->KingSQ[pos->side], pos->side^1, pos);
    if (InCheck == TRUE) depth++;
    //create move list and generate all moves for current position
    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int MoveNum = 0;
    int Legal = 0;  //check for number of legal moves in order to detect checkmate/stalemate
    int OldAlpha = alpha; //in iterative deepening, checks if next depth found a better move than last iteration's alpha
    int BestMove = NOMOVE;
    int Score = -INFINITY;

    //probe the pv table to check if we are in the principal variation
    int PvMove = ProbePvTable(pos);
    //if so, prioritize the move leading to the principal variation in the move search
    if(PvMove != NOMOVE){
        for(MoveNum = 0; MoveNum < list->count; MoveNum++){
            if(list->moves[MoveNum].move == PvMove){
                list->moves[MoveNum].score = 2000000;
                break;
            }
        }
    }

    for(MoveNum = 0; MoveNum < list->count; MoveNum++){
        //reorder moves for search based on mvv-lva scores
        PickNextMove(MoveNum, list);
        //if move cannot be made, continue to next move in list
        if( !MakeMove(pos, list->moves[MoveNum].move)){
            continue;
        }
        Legal++;
        //call algorithm with next depth assuming the current move was played
        Score = -AlphaBeta(-beta, -alpha, depth-1, pos, info, TRUE);
        TakeMove(pos);

        if(info->stopped == TRUE){
            return 0;
        }

        if(Score > alpha){
            //beta cutoff
            if(Score >= beta){
                //check if first fail high case
                if(Legal == 1) info->fhf++;
                info-> fh++;
                //check for search killer
                //search killers are quiet moves that cause a beta cutoff
                if(!(list->moves[MoveNum].move & MFLAGCAP)){
                    pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
                    pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;
                }
                return beta;
            }
            //else better alpha score was found
            alpha = Score;
            BestMove = list->moves[MoveNum].move;
            if(!(list->moves[MoveNum].move & MFLAGCAP)){
                pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][pos->pieces[TOSQ(BestMove)]] += depth;
            }
        }
    }

    //check for stalemate/checkmate
    if(Legal == 0){
        if(InCheck){
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

        //exit search once limit is reached
        if(info->stopped == TRUE){
            break;
        }

        //store best variation and best next move
        pvMoves = GetPvLine(currDepth, pos);
        bestMove = pos->PvArray[0];
        if(info->GAME_MODE == UCIMODE){
            printf("info score cp %d depth %d node %ld time %d", bestScore, bestScore, currDepth, info->nodes, GetTimeMs()-info->startTime);
        }
        //XBOARD protocol
        else if(info->GAME_MODE == XBOARDMODE && info->POST_THINKING == TRUE){
            printf("%d %d %d %d", currDepth, bestScore, info->nodes, (GetTimeMs()-info->startTime)/10);
        }
        //console mode
        else if(info->POST_THINKING == TRUE){
            printf("score:%d depth:%d nodes:%ld time:%d(ms) ", bestScore, currDepth, info->nodes, (GetTimeMs()-info->startTime));
        }
        //UCI protocol
        if(info->GAME_MODE == UCIMODE || info->POST_THINKING == TRUE){
            pvMoves = GetPvLine(currDepth, pos);
            if(!info->GAME_MODE == XBOARDMODE){
                printf("pv");
            }
            for(pvNum = 0; pvNum < pvMoves; pvNum++){
                printf(" %s", PrMove(pos->PvArray[pvNum]));
            }
            printf("\n");
        }
    }

    //make move according to protocol
    if(info->GAME_MODE == UCIMODE){
        printf("bestmove %s\n", PrMove(bestMove));
    }
    else if(info->GAME_MODE == XBOARDMODE){
        printf("move %s\n", PrMove(bestMove));
        MakeMove(pos, bestMove);
    }
    else{
        printf("\n** Vice makes move: %s **", PrMove(bestMove));
        MakeMove(pos, bestMove);
        PrintBoard(pos);
    }
}