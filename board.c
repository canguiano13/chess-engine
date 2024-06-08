//used to set modify the current chess board position
#include <stdio.h>
#include "defs.h"

int Parse_Fen(char *fen, S_BOARD *pos){
    ASSERT(fen!=NULL);
    ASSERT(pos!=NULL);
    
    int rank = RANK_8;
    int file = FILE_A;
    int piece = 0;
    int count = 0;
    int sq64 = 0;
    int sq120 = 0;
    int i = 0;

    ResetBoard(pos);

    //parse through fen notation
    while((rank >= RANK_1) && *fen){
        count = 1;
        //check next character in fen and add piece to board
        switch (*fen){
            //select black piece based on FEN character
            case 'p': piece = bP; break; //select a black pawn
            case 'r': piece = bR; break; //select a black rook
            case 'n': piece = bN; break; //select a black knight
            case 'b': piece = bB; break; //select a black bishop
            case 'q': piece = bQ; break; //select a black queen
            case 'k': piece = bK; break; //select a black king
            //select black piece based on FEN character
            case 'P': piece = wP; break; //select a white pawn
            case 'R': piece = wR; break; //select a white rook
            case 'N': piece = wN; break; //select a white knight
            case 'B': piece = wB; break; //select a white bishop
            case 'Q': piece = wQ; break; //select a white queen
            case 'K': piece = wK; break; //select a white king
            //parse consecutive empty squares
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;  //add empty piece
                count = *fen - '0'; //get empty square count
                break;

            case '/':
            case ' ':
                rank--; //move to previous rank
                file = FILE_A; //reset file
                fen++; //move to next character in FEN
                continue;
            default:
                printf("FEN ERROR \n");
                return -1;
        }

        for(i=0; i<count; i++){
            sq64 = rank * 8 + file;
            sq120 = SQ120(sq64);
            if (piece != EMPTY){
                pos->pieces[sq120] = piece;
            }
            file++;
        }

        fen++; //move to next character in FEN

    }//END OF WHILE

    //check active side
    ASSERT(*fen == 'w' || *fen == 'b');
    pos->side = (*fen == 'w') ? WHITE:BLACK;
    fen+=2; //then move to castling permissions

    //check castling permissions
    for(i=0; i<4; i++){
        if(*fen = ' ') break; //break once no more permissions are stated
        switch(*fen){
            case 'K': pos->castlePerm |= WKCA; break;   //USE bitwise OR to establish castling permissions
            case 'Q': pos->castlePerm |= WQCA; break;
            case 'k': pos->castlePerm |= BKCA; break;
            case 'q': pos->castlePerm |= BQCA; break;
            default:                           break;
        }
        //move to next char in FEN
        fen++;
    }
    fen++; //move to en passant square
    ASSERT(pos->castlePerm>=0 && pos->castlePerm<=15);
    
    //set en passant square if any exists
    if(*fen != '-'){
        file = fen[0] - 'a';
        rank = fen[1] - '1';

        ASSERT(file>=FILE_A && file<=FILE_H);
        ASSERT(rank>=RANK_1 && rank<=RANK_8);

        pos->enPassant = FR2SQ(file, rank);
    }

    //generate positionID based on current position;
    pos->posKey = GeneratePosKey(pos);

    return 0;
}

void ResetBoard(S_BOARD *pos){
    int index = 0;

    //mark all squares in full-board representation as offboard
    for(index = 0; index < BRD_SQ_NUM; index++){
        pos->pieces[index] = OFFBOARD;
    }
    //set on-board squares to empty
    for(index = 0; index < 64; index++){
        pos->pieces[SQ120(index)] = EMPTY;
    }
    //reset individual piece trackers
    for(index = 0; index < 3; index++){
        pos->bigPce[index] = 0;
        pos->majPce[index] = 0;
        pos->minPce[index] = 0;
        pos->pawns[index] = 0ULL;
    }
    //reset overall piece counter
    for(index = 0; index < 13; index++){
        pos->pceNum[index] = 0;
    }
    //remove kings from board
    pos->KingSQ[WHITE] = pos->KingSQ[BLACK] = NO_SQ;
    //reset castling permissions, fifty move tracker, enPassant square
    pos->side = BOTH;
    pos->enPassant = NO_SQ;
    pos->fiftyMove = 0;
    pos->castlePerm = 0;
    //reset current/previous move counters
    pos->ply = 0;
    pos->hisPly = 0;
    //reset positionID
    pos->posKey = 0;

}