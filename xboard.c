#include <stdio.h>
#include "defs.h"
#include <string.h>

//detect three-fold repetition
int ThreeFoldRep(const S_BOARD *pos){
    int i = 0;
    int r = 0;
    for(i = 0; i<pos->hisPly; i++){
        if(pos->history[i].posKey == pos->posKey){
            r++;
        }
    }
    return r;
}

//detect draw by insufficient material
int DrawMaterial(const S_BOARD *pos){
    if(pos->pceNum[wP] || pos->pceNum[bP]) return FALSE;
    if(pos->pceNum[wQ] || pos->pceNum[bQ] || pos->pceNum[wR] || pos->pceNum[bR]) return FALSE;
    if(pos->pceNum[wB] > 1 || pos->pceNum[bB] > 1) return FALSE;
    if(pos->pceNum[wN] > 1 || pos->pceNum[bN] > 1) return FALSE;
    if(pos->pceNum[wN] && pos->pceNum[wB]) return FALSE;
    if(pos->pceNum[bN] && pos->pceNum[bB]) return FALSE;

    return TRUE;
}

int CheckResult(S_BOARD *pos){
    //check for draw
    if(pos->fiftyMove > 100){
        printf("1/2-1/2 (fifty move rule (claimed by VICE))\n"); return TRUE;
    }
    if(ThreeFoldRep(pos) >= 2){
        printf("1/2-1/2 (3-fold repetition (claimed by VICE))\n"); return TRUE;
    }
    if(DrawMaterial(pos) == TRUE){
        printf("1/2-1/2 (insufficient material (claimed by VICE))\n"); return TRUE;
    }
    //check for stalemate/checkmate
    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);
    int MoveNum = 0;
    int found = 0;
    for(MoveNum = 0; MoveNum < list->count; MoveNum++){
        if( !MakeMove(pos, list->moves[MoveNum].move) ){
            continue;
        }
        found++;
        TakeMove(pos);
        break;
    }
    if(found != 0) return FALSE;
    int InCheck = SqAttacked(pos->KingSQ[pos->side], pos->side^1, pos);
    if(InCheck == TRUE){
        if(pos->side == WHITE){
            printf("0-1 (black mates (claimed by VICE))\n"); return TRUE;
        }
        else{
            printf("1-0 (white mates (claimed by VICE))\n"); return TRUE;
        }
    }
    else{
        printf("\n1/2-1/2 (stalemate (claimed by VICE))\n"); return TRUE;
    }

    return FALSE;
}

void PrintOptions(){
    printf("feature ping=1 setboard=1 colors=0 usermove=1\n");
    printf("feature done=1\n");
}
void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info){
    //set xboard protocol
    info->GAME_MODE = XBOARDMODE;
    info->POST_THINKING = TRUE;

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    PrintOptions();

    int depth = -1, movestogo[2] = {30, 30}, movetime = -1;
    int time = -1, inc = 0;
    int engineSide = BOTH;
    int timeLeft;
    int sec;
    int mps;
    int move = NOMOVE;
    int i, score;
    char inBuf[80], command[80];

    while(TRUE){
        fflush(stdout);

        if(pos->side == engineSide && CheckResult(pos) == FALSE){
            info->startTime = GetTimeMs();
            info->depth = depth;

            if(time != -1){
                info->timeSet = TRUE;
                time /= movestogo[pos->side];
                time -= 50;
                info->stopTime = info->startTime+time+inc;
            }

            if(depth == -1 || depth > MAX_SEARCH_DEPTH){
                info->depth = MAX_SEARCH_DEPTH;
            }

            printf("time:%d start:%d stop:%d depth:%d timeset:%d movestogo%d mps:%d\n", 
                    time, info->startTime, info->stopTime, info->depth, info->timeSet, movestogo[pos->side], mps);
            SearchPosition(pos, info);

            if(mps != 0){
                movestogo[pos->side^1]--;
                if(movestogo[pos->side^1] < 1){
                    movestogo[pos->side^1] = mps;
                }
            }
        }

        fflush(stdout);
        memset(&inBuf[0], 0, sizeof(inBuf));
        fflush(stdout);
        if(!fgets(inBuf, 80, stdin)){
            continue;
        }

        sscanf(inBuf, "%s", command);

        if(!strcmp(command, "quit")){
            break;
        }
        if(!strcmp(command, "force")){
            engineSide = BOTH;
            continue;
        }
        if(!strcmp(command, "protover")){
            PrintOptions();
            continue;
        }
        if(!strcmp(command, "sd")){
            sscanf(inBuf, "sd %d", &depth);
            continue;
        }
        if(!strcmp(command, "st")){
            sscanf(inBuf, "st %d", &movetime);
            time *= 10;
            printf("DEBUG time:%d\n", time);
            break;
        }
        if(!strcmp(command, "level")){
            sec = 0;
            movetime = -1;
            if(sscanf(inBuf, "level %d %d %d", &mps, &timeLeft, &inc) != 3){
                sscanf(inBuf, "level %d %d:%d %d", &mps, &timeLeft, &sec, &inc);
                printf("DEBUG level with :\n");
            }
            else{
                printf("DEBUG level without :\n");
            }
            timeLeft *= 60000;
            timeLeft += sec*1000;
            movestogo[0] = movestogo[1] = 30;
            if(mps != 0){
                movestogo[0] = movestogo[1] = mps;
            }
            time = -1;
            printf("DEBUG level timeLeft: %d movesToGo:%d inc:%d mps%d\n", timeLeft, movestogo[0], inc, mps);
            continue;
        }

        if(!strcmp(command, "ping")){
            printf("pong%s\b", inBuf+4);
            continue;
        }
        if(!strcmp(command, "new")){
            ClearPvTable(pos->PvTable);
            engineSide = BLACK;
            ParseFen(START_FEN, pos);
            depth = -1;
            continue;
        }
        if(!strcmp(command, "setboard")){
            engineSide = BOTH;
            ParseFen(inBuf+9, pos);
            continue;
        }
        if(!strcmp(command, "go")){
            engineSide = pos->side;
            continue;
        }
        if(!strcmp(command, "usermove")){
            movestogo[pos->side]--;
            move = ParseMove(inBuf+9, pos);
            if(move == NOMOVE) continue;
            MakeMove(pos, move);
            pos->ply=0;
        }
    }

}

void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info){
    printf("\n***********************************************");
    printf("\nWelcome to VICE in Console Mode!\n");
    printf("Type \"help\" for commands or begin the game by playing a move.\n");
    
    info->GAME_MODE = CONSOLEMODE;
    info->POST_THINKING = TRUE;

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    int depth = MAX_SEARCH_DEPTH;
    int moveTime = 3000;
    int engineSide = BOTH;
    int move = NOMOVE;
    char inBuf[80], command[80];

    engineSide = BLACK;
    ParseFen(START_FEN, pos);
    
    while(TRUE){
        fflush(stdout);

        if(pos->side == engineSide && CheckResult(pos) == FALSE){
            info->startTime = GetTimeMs();
            info->depth = depth;

            if(moveTime != 0){
                info->timeSet = TRUE;
                info->stopTime = info->startTime + moveTime;
            }
            printf("VICE is thinking...\n");
            SearchPosition(pos, info);
        }
        printf("\nEnter command or next move:\n");
        printf("VICE > ");

        fflush(stdout);

        memset(&inBuf[0], 0, sizeof(inBuf));
        fflush(stdout);
        if(!fgets(inBuf, 80, stdin)){
            continue;
        }
        sscanf(inBuf, "%s", command);
        
        //perform action based on command
        if(!strcmp(command, "help")){
            printf("\n***********************************************\n");
            printf("Commands:\n");
            printf("\"quit\"     - quit game and exit VICE:\n");
            printf("\"take\"     - take back your move\n");
            printf("\"black\"    - play a new game as black\n");
            printf("\"white\"    - play a new game as white\n");
            printf("\"new\"      - start new game with the same color pieces\n");
            printf("\"eval\"     - print VICE's evaluation of the current position\n");
            printf("\"set F\"    - set the board to a custom position F, where F is a FEN. Both sides keep the same color.");
            printf("\"force\"    - computer will not think\n");
            printf("\"go\"       - set computer thinking\n");
            printf("\"nopost\"   - do not show thinking\n");
            printf("\"post\"     - show thinking\n");
            printf("\"print\"    - show board:\n");
            printf("\"mirror\"   - print a mirrored version of the current board and its evaluation\n");
            printf("\"depth x\"  - set thinking depth to x:\n");
            printf("\"time x\"   - set thinking time to x seconds. depth will apply if set\n");
            printf("\"view\"   - show current depth and movetime settings\n");
           
            printf("\nNOTE: to reset time and depth, set to 0\n");
            printf("\n**ENTER MOVES USING ALGEBRAIC NOTATION (e.g. b7b8q)**\n\nHave Fun!\n");
            printf("\n***********************************************\n");
            continue;
        }
        if(!strcmp(command, "quit")){
            printf("Quitting...\n");
            printf("\nThanks for Playing!\n\n");
            info->quit = TRUE;
            break;
        }
        if(!strcmp(command, "take")){
            printf("Taking back your move...\n");
            //take back computer's move
            TakeMove(pos);
            //take back player's move
            TakeMove(pos);
            PrintBoard(pos);
            continue;
        }
        if(!strcmp(command, "black")){
            printf("Starting new game as black...\n");
            ClearPvTable(pos->PvTable);
            engineSide=WHITE;
            ParseFen(START_FEN, pos);
            continue;
        }
        if(!strcmp(command, "white")){
            printf("Starting new game as white...\n");
            printf("Game started. Enter a move to begin.\n");
            ClearPvTable(pos->PvTable);
            engineSide=BLACK;
            ParseFen(START_FEN, pos);
            continue;
        }
        if(!strcmp(command, "new")){
            printf("Restarting game...\n");
            ClearPvTable(pos->PvTable);
            engineSide=pos->side^1;
            if(engineSide == BLACK){
                printf("Game Restarted. Enter a move to begin.\n");
            }
            ParseFen(START_FEN, pos);
            continue;
        }
        if(!strcmp(command, "eval")){
            printf("VICE evaluates the position with score: %d\n", EvalPosition(pos));
            continue;
        }
        if(!strcmp(command, "set")){
            ParseFen(inBuf+4, pos);
            printf("\nSetting custom position...");
            printf("\nBoard is now:");
            PrintBoard(pos);
            continue;
        }
        if(!strcmp(command, "force")){
            printf("Computer will stop thinking. Enter \"go\" to resume thinking.\n");
            engineSide = BOTH;
            continue;
        }
        if(!strcmp(command, "go")){
            printf("Resuming computer's thinking...\n");
            engineSide = pos->side;
            continue;
        }
        if(!strcmp(command, "post")){
            printf("Computer's thoughts will now be posted.\n");
            info->POST_THINKING = TRUE;
            continue;
        }
        if(!strcmp(command, "nopost")){
            printf("Computer's thoughts will NOT be posted.\n");
            info->POST_THINKING = FALSE;
            continue;
        }
        if(!strcmp(command, "print")){
            printf("Printing...\n");
            PrintBoard(pos);
            continue;
        }
        if(!strcmp(command, "mirror")){
            printf("Board is currently:");
            PrintBoard(pos);
            printf("Mirroring board...\n");
            MirrorBoard(pos);
            printf("Mirroring complete. Board is now:");
            PrintBoard(pos);
            printf("\nVICE scores the mirrored board as: %d\n", EvalPosition(pos));
            printf("\nReturning to regular board...");
            MirrorBoard(pos);
            PrintBoard(pos);
            continue;
        }
        if(!strcmp(command, "view")){
            if(depth == MAX_SEARCH_DEPTH) printf("depth not set ");
            else printf("depth %d", depth);

            if(moveTime != 0) printf(" movetime %ds\n", moveTime/1000);
            else printf(" movetime not set\n");

            continue;
        }
        if(!strcmp(command, "depth")){
            printf("Setting depth...\n");
            sscanf(inBuf, "depth %d", &depth);
            if(depth == 0) depth = MAX_SEARCH_DEPTH;
            continue;
        }
        if(!strcmp(command, "time")){
            printf("Setting time...\n");
            sscanf(inBuf, "time %d", &moveTime);
            moveTime *= 1000;
            continue;
        }

        //if no command was made, make a move or respond with error message
        move = ParseMove(inBuf, pos);
        if(move == NOMOVE){
            printf("Command unknown or ilegal move: %s\n", inBuf);
            continue;
        }
        MakeMove(pos, move);
        pos->ply = 0;
    }
}




