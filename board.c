//used to set modify the current chess board position
#include <stdio.h>
#include "defs.h"

int CheckBoard(const S_BOARD *pos){
    //setup temporary variables to hold information mirroring current position
    int t_pceNum[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int t_bigPce[2] = {0, 0};
    int t_majPce[2] = {0, 0};
    int t_minPce[2] = {0, 0};
    int t_material[2] = {0, 0};

    int sq64, t_piece, t_pce_num, sq120, color, pcount;

    U64 t_pawns[3] = {0ULL, 0ULL, 0ULL};

    t_pawns[WHITE] = pos->pawns[WHITE];
    t_pawns[BLACK] = pos->pawns[BLACK];
    t_pawns[BOTH] = pos->pawns[BOTH];

    //check piece lists
    for(t_piece = wP; t_piece <= bK; ++t_piece){
        for(t_pce_num = 0; t_pce_num < pos->pceNum[t_piece]; ++t_pce_num){
            sq120 = pos->pList[t_piece][t_pce_num];
            ASSERT(pos->pieces[sq120] == t_piece);
        }
    }

    //check piece count and other counters
    for(sq64 = 0; sq64 < 64; ++sq64){
        sq120 = SQ120(sq64);
        t_piece = pos->pieces[sq120];
        t_pceNum[t_piece]++;
        color = PieceCol[t_piece];

        if(PieceBig[t_piece] == TRUE) t_bigPce[color]++;
        if(PieceMin[t_piece] == TRUE) t_minPce[color]++;
        if(PieceMaj[t_piece] == TRUE) t_majPce[color]++;

        t_material[color] += PieceVal[t_piece];
    }

    for(t_piece = wP; t_piece <= bK; t_piece++){
        ASSERT(t_pceNum[t_piece] == pos->pceNum[t_piece]);
    }

    //check bitboards count
    pcount = CNT(t_pawns[WHITE]);
    ASSERT(pcount == pos->pceNum[wP]);
    pcount = CNT(t_pawns[BLACK]);
    ASSERT(pcount == pos->pceNum[bP]);
    pcount = CNT(t_pawns[BOTH]);
    ASSERT(pcount == pos->pceNum[wP] + pos->pceNum[bP]);

    //check bitboards squares
    while(t_pawns[WHITE]){
        sq64 = POP(&t_pawns[WHITE]);
        ASSERT(pos->pieces[SQ120(sq64)] == wP);
    }
    while(t_pawns[BLACK]){
        sq64 = POP(&t_pawns[BLACK]);
        ASSERT(pos->pieces[SQ120(sq64)] == bP);
    }
    while(t_pawns[BOTH]){
        sq64 = POP(&t_pawns[BOTH]);
        ASSERT(     (pos->pieces[SQ120(sq64)] == wP) || (pos->pieces[SQ120(sq64)] == bP)    );
    }

    ASSERT(t_material[WHITE]==pos->material[WHITE] && t_material[BLACK]==pos->material[BLACK]);
    ASSERT(t_minPce[WHITE]==pos->minPce[WHITE] && t_minPce[BLACK]==pos->minPce[BLACK]);
    ASSERT(t_majPce[WHITE]==pos->majPce[WHITE] && t_majPce[BLACK]==pos->majPce[BLACK]);
    ASSERT(t_bigPce[WHITE]==pos->bigPce[WHITE] && t_bigPce[BLACK]==pos->bigPce[BLACK]);

    ASSERT(pos->side==WHITE || pos->side==BLACK);
    ASSERT(GeneratePosKey(pos) == pos->posKey);

    ASSERT(pos->enPassant == NO_SQ ||
            (RanksBrd[pos->enPassant] == RANK_6 && pos->side == WHITE) ||
            (RanksBrd[pos->enPassant] == RANK_3 && pos->side == BLACK));

    ASSERT(pos->pieces[pos->KingSQ[WHITE]] == wK);
    ASSERT(pos->pieces[pos->KingSQ[BLACK]] == bK);

    return TRUE;
}

//build or update piece lists
void UpdateListsMaterial(S_BOARD *pos){
    int piece, sq, index, color;
    
    //iterate over board and extract information from the pieces currently on the board
    for(index = 0; index < BRD_SQ_NUM; index++){
        sq = index;
        piece = pos->pieces[index];
        if(piece != OFFBOARD && piece != EMPTY){
            color = PieceCol[piece];

            //add to corresponding piece counter if necessary
            if(PieceBig[piece] == TRUE) pos->bigPce[color]++;
            if(PieceMin[piece] == TRUE) pos->minPce[color]++;
            if(PieceMaj[piece] == TRUE) pos->majPce[color]++;
            
            //add to side's total material
            pos->material[color] += PieceVal[piece];

            ASSERT(pos->pceNum[piece] < 10 && pos->pceNum[piece] >= 0);

            pos->pList[piece][pos->pceNum[piece]] = sq;
            pos->pceNum[piece]++;

            //set king squares
            if(piece == wK) pos->KingSQ[WHITE] = sq;
            if(piece == bK) pos->KingSQ[BLACK] = sq;

            if(piece == wP){
                SETBIT(pos->pawns[WHITE], SQ64(sq));
                SETBIT(pos->pawns[BOTH], SQ64(sq));
            }
            else if(piece == bP){
                SETBIT(pos->pawns[BLACK], SQ64(sq));
                SETBIT(pos->pawns[BOTH], SQ64(sq));
            }
        }

    }
}

//update piece lists based on FEN record
int ParseFen(char *fen, S_BOARD *pos){
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
    while((rank >= RANK_1) && (*fen)){
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
                fen++; //move to next character in FEN
                file = FILE_A; //reset file
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
        if(*fen == ' ') break; //break once no more permissions are stated
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

    ASSERT(pos->castlePerm >=0 && pos->castlePerm <=15);
    
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

    //update piece lists arrays
    UpdateListsMaterial(pos);

    return 0;
}

void MirrorBoard(S_BOARD *pos){
    int tempPiecesArray[64];
    int tempSide=pos->side^1;
    int SwapPiece[13] = {EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK};
    int tempCastlePerm = 0;
    int tempEnPassant = NO_SQ;

    int sq;
    int tp;

    //mirror castling permissions
    if(pos->castlePerm & WKCA) tempCastlePerm |= BKCA;
    if(pos->castlePerm & WQCA) tempCastlePerm |= BQCA;
    if(pos->castlePerm & BKCA) tempCastlePerm |= WKCA;
    if(pos->castlePerm & BQCA) tempCastlePerm |= WQCA;

    //mirror en passant square
    if(pos->enPassant != NO_SQ){
        tempEnPassant = SQ120(MIRROR64(SQ64(pos->enPassant)));
    }

    //mirror pieces
    for(sq = 0; sq < 64; sq++){
        tempPiecesArray[sq] = pos->pieces[SQ120(MIRROR64(sq))];
    }

    ResetBoard(pos);

    for(sq = 0; sq < 64; sq++){
        tp = SwapPiece[tempPiecesArray[sq]];
        pos->pieces[SQ120(sq)] = tp;
    }

    pos->side = tempSide;
    pos->castlePerm = tempCastlePerm;
    pos->enPassant = tempEnPassant;

    pos->posKey = GeneratePosKey(pos);

    UpdateListsMaterial(pos);

    ASSERT(CheckBoard(pos));
}

//reset all pieces on board
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
    for(index = 0; index < 2; index++){
        pos->bigPce[index] = 0;
        pos->majPce[index] = 0;
        pos->minPce[index] = 0;
        pos->material[index] = 0;
    }
    for(index = 0; index < 3; ++index){
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
    pos->posKey = 0LL;

}

void PrintBoard(S_BOARD *pos){
    int sq, rank, piece;
    int file = FILE_A;

    printf("\n\nGAME BOARD:\n");

    for(rank=RANK_8; rank>=RANK_1; rank--){
        printf("%d  ", rank+1); //add 1 to rank since they are zero-indexed
        for(file = FILE_A; file <= FILE_H; file++){
            sq = FR2SQ(file, rank);
            piece = pos->pieces[sq];
            printf("%3c", PceChar[piece]);
        }
        printf("\n");
    }

    printf("\n  ");
    for(file = FILE_A; file <= FILE_H; file++){
        printf("%3c", 'a' + file);
    }

    printf("\n");
    // printf("side: %c\n", SideChar[pos->side]);
    // printf("enPas: %d\n", pos->enPassant);
    // printf("castle:%c%c%c%c\n", 
    //     pos->castlePerm & WKCA ?'K':'-',
    //     pos->castlePerm & WQCA ?'Q':'-',
    //     pos->castlePerm & BKCA ?'k':'-',
    //     pos->castlePerm & BQCA ?'q':'-');
    // printf("PosKey: %11X\n", pos->posKey);
}