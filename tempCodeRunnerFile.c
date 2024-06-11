ParseFen(TEMP_FEN, board);
    PrintBoard(board);
    printf("\nwp: \n");
    PrintBitBoard(board->pawns[WHITE]);
    printf("\nbP: \n");
    PrintBitBoard(board->pawns[BLACK]);
    printf("\nboth: \n");
    PrintBitBoard(board->pawns[BOTH]);
    ASSERT(CheckBoard(board));