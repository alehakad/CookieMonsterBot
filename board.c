#include <stddef.h> /* size_t */
#include <stdlib.h> /* malloc */

#ifndef NDEBUG
#include <stdio.h> /* printf */
#endif

#include "board.h" /* board header */
/* 
- board will be represented by bitboards of 64 bits for every type of figure and color;
- also all figures bitboard is saved
*/
struct color_board
{
    bitmap_t rooks;
    bitmap_t pawns;
    bitmap_t knights;
    bitmap_t king;
    bitmap_t queen;
    bitmap_t bishops;
    bitmap_t all;
};

struct board
{
    color_board_t whites;
    color_board_t blacks;
    bitmap_t all;
};


static bitmap_t ClearRankLUT[N_SQUARES];
static bitmap_t MaskRankLUT[N_SQUARES];
static bitmap_t ClearFileLUT[N_SQUARES];
static bitmap_t MaskFileLUT[N_SQUARES];
static bitmap_t PieceLUT[N_SQUARES*N_SQUARES];
bitmap_t PawnAttacks[N_SQUARES*N_SQUARES][ALL];
bitmap_t KnightAttacks[N_SQUARES*N_SQUARES][ALL];

static int KnightsMoves[8][2] = {{2,1},{2,-1},{-2,1},{2,-1},{1,1},{1,-1},{1,2},{1,-2}};

int GetRank(int index)
{
    return index >> 3; /* /8 */
}

int GetFile(int index)
{
    return index & 7; /* %8 */
}

int GetIndex(int rank, int file)
{
    return (rank<<3) + file;
}


static void FillLuts()
{
    size_t i = 0, j = 0;
    bitmap_t bm = 0;

    /* mask ranks */
    for (i=0; i<8; ++i)
    {
        bm = 0L;
        for (j=0; j<8; ++j)
        {
            bm = BitBoardSetOn(bm, GetIndex(i, j));
        }
        MaskRankLUT[i] = bm;
    }

    /* clear ranks */
    for (i=0; i<8; ++i)
    {
        bm = BitBoardSetAll(bm);;
        for (j=0; j<8; ++j)
        {
            bm = BitBoardSetOff(bm, GetIndex(i, j));
        }
        ClearRankLUT[i] = bm;
    }

    /* mask files */
    for (i=0; i<8; ++i)
    {
        bm = 0L;
        for (j=0; j<8; ++j)
        {
            bm = BitBoardSetOn(bm, GetIndex(j, i));
        }
        MaskFileLUT[i] = bm;
    }

    /* clear files */
    for (i=0; i<8; ++i)
    {
        bm = BitBoardSetAll(bm);;
        for (j=0; j<8; ++j)
        {
            bm = BitBoardSetOff(bm, GetIndex(j, i));
        }
        ClearFileLUT[i] = bm;
    }

    /* piece lut */
    for (i=0; i<64; ++i)
    {
        bm = BitBoardSetOn(0, i);
        PieceLUT[i] = bm;
    }

}

static color_board_t FillColorBoard(color_t color)
{
    color_board_t board = {0};
    bitmap_t bm = 0;
    size_t i = 0;
    int rank = 0;
    int direction = 0;

    rank = (color == WHITE) ? 7 : 0;
    direction = (color == WHITE) ? -1 : 1;

    board.king = BitBoardSetOn(bm, GetIndex(rank,4));
    board.queen = BitBoardSetOn(bm, GetIndex(rank,3));
    board.bishops = BitBoardSetOn(BitBoardSetOn(bm,GetIndex(rank,2)),GetIndex(rank,5));
    board.knights = BitBoardSetOn(BitBoardSetOn(bm,GetIndex(rank,1)),GetIndex(rank,6));
    board.rooks = BitBoardSetOn(BitBoardSetOn(bm,GetIndex(rank,0)),GetIndex(rank,7));
    for (i=0; i<8; ++i)
    {
        bm = BitBoardSetOn(bm, GetIndex(rank+direction,i));
    }
    board.pawns = bm;

    board.all = board.king | board.queen | board.bishops | board.rooks | board.knights | board.pawns;

    return board;
}

static int CheckMoveInBoard(int rank, int file)
{
    return rank>=0 && rank<8 && file>=0 && file<8;
}

static int CheckMove(int rank, int file, color_t color)
{
    return (CheckMoveInBoard(rank, file));
}



void GenerateMoves(board_t board, color_t color)
{

}


/* to pre generate and save in LUT possible pawn attacks from current square */
bitmap_t GeneratePawnAttacks(int square, color_t color)
{
    bitmap_t attack = 0L;

    bitmap_t board = 0L;

    board = BitBoardSetOn(board, square);

    if (!color) /* white */
    {
        if ((board >> 7) & ClearFileLUT[0])
        {
            attack |= (board >> 7);
        }
        if ((board >> 9) & ClearFileLUT[7])
        {
            attack |= (board >> 9);
        }
    }
    else
    {
        if ((board << 7) & ClearFileLUT[7])
        {
            attack |= (board << 7);
        }
        if ((board << 9) & ClearFileLUT[0])
        {
            attack |= (board << 9);
        }
    }

    return attack;

}

/* init lut of pawn attacks for every square */
void InitPawnAttacks()
{
    int square = 0;

    for (; square<64; ++square)
    {
        PawnAttacks[square][WHITE] = GeneratePawnAttacks(square, WHITE);
        PawnAttacks[square][BLACK] = GeneratePawnAttacks(square, BLACK);
    }

}


/* 
- generate knight attacks
- here for certain square all 8 direction move are tried and checked if they are on board
- also possible to use bitwise offsets: 17, 15,...
 */
bitmap_t GenerateKnightAttacks(int square, color_t color)
{
    int i = 0;
    int cur_index = 0;
    int x = 0, y = 0, new_x = 0, new_y = 0;
    bitmap_t attack = 0L;
    bitmap_t board = 0L;

    x = GetRank(cur_index);
    y = GetFile(cur_index);

    for (i=0; i<8; ++i)
    {
        new_x = x + KnightsMoves[i][0];
        new_y = y + KnightsMoves[i][1];

        if (CheckMoveInBoard(new_x, new_y))
        {
            BitBoardSetOn(board, GetIndex(new_x, new_y));
        }
        
    }

    return board;
}

/* init lut of pawn attacks for every square */
void InitKnightAttacks()
{
    int square = 0;

    for (; square<64; ++square)
    {
        KnightAttacks[square][WHITE] = GenerateKnightAttacks(square, WHITE);
        KnightAttacks[square][BLACK] = GenerateKnightAttacks(square, BLACK);

    }

}


static char GetFigure(board_t *board, color_t color, int square)
{
    color_board_t c_board = (color == WHITE) ? board->whites : board->blacks;

    if (BitBoardIsOn(c_board.pawns, square))
    {
        return 'p';
    }
    if (BitBoardIsOn(c_board.knights, square))
    {
        return 'n';
    }
    if (BitBoardIsOn(c_board.rooks, square))
    {
        return 'r';
    }
    if (BitBoardIsOn(c_board.queen, square))
    {
        return 'q';
    }
    if (BitBoardIsOn(c_board.king, square))
    {
        return 'k';
    }
    if (BitBoardIsOn(c_board.bishops, square))
    {
        return 'b';
    }
    if (BitBoardIsOn(c_board.all, square))
    {
        return '1';
    }

    return '0';
}





#ifndef NDEBUG


void PrintBitsBoard(bitmap_t board)
{
    int i = 0, j = 0;
    int square = 0;

    for (i=0; i<8; ++i)
    {
        for (j=0; j<8; ++j)
        {
            if (!j)
            {
                printf("  %d  ", 8-i);
            }
            square = GetIndex(i,j);
            printf("%d ", BitBoardIsOn(board, square));
        }
        puts("");
    }
    printf("\n     a b c d e f g h \n");
    puts("");
}

void PrintColorBoard(board_t *board, color_t color)
{
    int i = 0, j = 0;
    int square = 0;
    char figure = 0;

    for (i=0; i<8; ++i)
    {
        for (j=0; j<8; ++j)
        {
            if (!j)
            {
                printf("  %d  ", 8-i);
            }
            square = GetIndex(i,j);
            figure = GetFigure(board, color, square);
            if (color == ALL && GetFigure(board, color, square) == '0')
            {
                figure = GetFigure(board, OPOSITE_COLOR(color), square);
            }
            printf("%c ",figure);
        }
        puts("");
    }
    printf("\n     a b c d e f g h \n");
    puts("");
}


void PrintBoard(board_t *board)
{
    puts("White board is\n");
    PrintColorBoard(board, WHITE);
    puts("Black board is\n");
    PrintColorBoard(board, BLACK);
    puts("Full board is\n");
    PrintColorBoard(board, ALL);
}

#endif



board_t *CreateBoard()
{
    board_t *board = NULL;  

    board = malloc(sizeof(board_t));
    if (!board)
    {
        return board;
    }
    
    board->whites = FillColorBoard(WHITE);
    board->blacks = FillColorBoard(BLACK);
    board->all = board->whites.all | board->blacks.all;

    /* init luts for files and ranks */
    FillLuts();

    /* init attacks luts for pieces */
    InitPawnAttacks();
    InitKnightAttacks();

    return board;
}

void DestroyBoard(board_t *board)
{
    free(board);
}