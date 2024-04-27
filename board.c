#include <stddef.h> /* size_t */
#include <stdlib.h> /* malloc */
#include <stdlib.h> /* rand */
#include <time.h> /* time */
#include <string.h> /* memset */


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
bitmap_t KnightAttacks[N_SQUARES*N_SQUARES];
bitmap_t KingAttacks[N_SQUARES*N_SQUARES];

/* number of set bits from each position */
int BishopNMoves[64] = {0};
int RookNMoves[64] = {0};

/* magic numbers */
static bitmap_t BishopMagicNumbers[64] = 
{
    0x10c4048800000082UL,
    0x10800100021400UL,
    0x1420810000a00000UL,
    0x4a80040000042UL,
    0x300918100000800UL,
    0x5005000a8080403UL,
    0x20100200700080UL,
    0x6400411200444402UL,
    0x1800c0232440040UL,
    0x3200a40580200001UL,
    0x8048a000080000UL,
    0x2022188000040120UL,
    0x34840818000002UL,
    0x1400001004150000UL,
    0x1800101200UL,
    0x42018002d80300aUL,
    0x4004500a30401002UL,
    0x102180b0004121UL,
    0x4001820403000062UL,
    0x1520040030002800UL,
    0x100000008b1080UL,
    0x2080248200080802UL,
    0x4892000540400048UL,
    0x2008000004069210UL,
    0x504010a040410720UL,
    0xc1000422802480UL,
    0x8a80090000400402UL,
    0x80c002000200000UL,
    0x620020000348940UL,
    0x8009008000UL,
    0x8101282402200aUL,
    0x2902001841ac02e0UL,
    0x4018a4000100200UL,
    0x231002a00084088UL,
    0x12040600810c880UL,
    0x480480004040015UL,
    0x200005a0c48002UL,
    0x98c1226f240020UL,
    0x2000084044001401UL,
    0xc408004a001040UL,
    0x54602100408810UL,
    0x2000819080200081UL,
    0x400040402c00004UL,
    0x62040981c0001032UL,
    0x2100222002400204UL,
    0x110400004a001911UL,
    0x8401002041c88UL,
    0x1901020400400510UL,
    0x409810042420040UL,
    0x8302420c01084200UL,
    0x40604002c0103000UL,
    0x48a400048c000008UL,
    0x4c0088200004c241UL,
    0x200008990040000UL,
    0x12081800c0104008UL,
    0x8012481000210800UL,
    0x24100080e0864004UL,
    0x4104204a60812001UL,
    0x4800000220422080UL,
    0x422008180c1020UL,
    0x2308102a00414400UL,
    0x1a02002820002020UL,
    0xd880020102a80104UL,
    0x100008008004980UL,
};

static bitmap_t RookMagicNumbers[64] = 
{
    0x4029000001000020UL,
    0x6400000100004UL,
    0x9480800100080200UL,
    0x10000802040003c0UL,
    0x1010430800024000UL,
    0x8a0080540180040UL,
    0x44000200a08801UL,
    0x410041240190800UL,
    0x248400008050201UL,
    0x8400000100002UL,
    0x16403402c2000000UL,
    0x1041468000001UL,
    0x20800000004000UL,
    0x2084100281084UL,
    0x2018a80200021000UL,
    0x1804000b00810100UL,
    0x10a0404800020UL,
    0xa008140900120000UL,
    0x800002808000284UL,
    0x1040100040421001UL,
    0x822042000UL,
    0xccc01088b5106UL,
    0x41001424280c0002UL,
    0x104800000103cUL,
    0x5200a10004100804UL,
    0x10c0d01405001000UL,
    0x4000000018002201UL,
    0x4000a6000004UL,
    0xc809020040000UL,
    0x4820028000000UL,
    0x88044000040b89UL,
    0x400808000c411a40UL,
    0x1424200012880200UL,
    0x2000001006481100UL,
    0xc40450010024000UL,
    0x400a00a030400a8UL,
    0x40a00018e000006UL,
    0x4100010080082040UL,
    0x401200c1002012UL,
    0xc004140010000800UL,
    0x2002080440700040UL,
    0x8000300019200UL,
    0x801042040301002UL,
    0x4160c1c040830060UL,
    0x8020293001200840UL,
    0x8005f00080000080UL,
    0x1080418c0012000UL,
    0x2081102000284000UL,
    0x2015002405000UL,
    0x420800000000100UL,
    0x8210200000020UL,
    0x200806200a008UL,
    0x100a000000818090UL,
    0x8050002148400UL,
    0x406800800c002000UL,
    0x148021000e00a0bUL,
    0x1800048020808a20UL,
    0x40000400400084UL,
    0x2000094011UL,
    0x42c0015000000028UL,
    0x202002000100608UL,
    0x802490000048000UL,
    0x8010200101420014UL,
    0xc00010000006UL,
};


static int KnightsMoves[8][2] = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
static int KingMoves[8][2] = {{1,1},{1,0},{1,-1},{-1,1},{-1,0},{-1,-1},{0,-1},{0,1}};


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

bitmap_t GenerateMagicNumber()
{
    return GetRandomU64Number() & GetRandomU64Number() & GetRandomU64Number();
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




/* 
- generate knight attacks
- here for certain square all 8 direction move are tried and checked if they are on board
- also possible to use bitwise offsets: 17, 15, 10, 6, -17, -15, -10, -6
 */
bitmap_t GenerateKnightAttacks(int square)
{
    int i = 0;
    int x = 0, y = 0, new_x = 0, new_y = 0;
    bitmap_t board = 0L;

    x = GetRank(square);
    y = GetFile(square);

    for (i=0; i<8; ++i)
    {
        new_x = x + KnightsMoves[i][0];
        new_y = y + KnightsMoves[i][1];

        if (CheckMoveInBoard(new_x, new_y))
        {
            board = BitBoardSetOn(board, GetIndex(new_x, new_y));
        }
        
    }

    return board;
}


bitmap_t GenerateKingAttacks(int square)
{
    int i = 0;
    int x = 0, y = 0, new_x = 0, new_y = 0;
    bitmap_t board = 0L;

    x = GetRank(square);
    y = GetFile(square);

    for (i=0; i<8; ++i)
    {
        new_x = x + KingMoves[i][0];
        new_y = y + KingMoves[i][1];

        if (CheckMoveInBoard(new_x, new_y))
        {
            board = BitBoardSetOn(board, GetIndex(new_x, new_y));
        }
        
    }

    return board;
}


/* get attacking rook squares except last file and index, to use further in magic bitboard */
bitmap_t GenerateRookAttacks(int square)
{
    bitmap_t board = 0L;

    int row = 0, file = 0;
    int x = 0, y = 0;

    x = GetRank(square);
    y = GetFile(square);

    for (row = x+1; row<=6; ++row)
    {
        board |= (1UL << GetIndex(row, y));
    }
    for (row = x-1; row>=1; --row)
    {
        board |= (1UL << GetIndex(row, y));
    }
    for (file=y-1; file>=1; --file)
    {
        board |= (1UL << GetIndex(x, file));
    }
    for (file=y+1; file<=6; ++file)
    {
        board |= (1UL << GetIndex(x, file));
    }

    return board;

}

/* get attacking bishop squares except last file and index, to use further in magic bitboard */
bitmap_t GenerateBishopAttacks(int square)
{
    bitmap_t board = 0L;

    int row = 0, file = 0;
    int x = 0, y = 0;

    x = GetRank(square);
    y = GetFile(square);

    for (row = x+1, file=y+1; row<=6 && file<=6; ++row, ++file)
    {
        board |= (1UL << GetIndex(row, file));
    }
    for (row = x-1, file=y+1; row>=1 && file<=6; --row, ++file)
    {
        board |= (1UL << GetIndex(row, file));
    }
    for (row = x+1, file=y-1; row<=6 && file>=1; ++row, --file)
    {
        board |= (1UL << GetIndex(row, file));
    }
    for (row = x-1, file=y-1; row>=1 && file>=1; --row, --file)
    {
        board |= (1UL << GetIndex(row, file));
    }

    return board;

}

/* generate bishop possible moves on current board with blocking pieces */
bitmap_t GenerateBishopAttacksOnTheFly(int square, bitmap_t blockers)
{
    bitmap_t attacks = 0L;

    int row = 0, file = 0, cur_index = 0;
    int x = 0, y = 0;

    x = GetRank(square);
    y = GetFile(square);

    for (row = x+1, file=y+1; row<=7 && file<=7; ++row, ++file)
    {
        cur_index = GetIndex(row, file);
        attacks |= (1UL << cur_index);
        if (1UL << cur_index & blockers)
        {
            break;
        }
    }
    for (row = x-1, file=y+1; row>=0 && file<=7; --row, ++file)
    {
        cur_index = GetIndex(row, file);
        attacks |= (1UL << cur_index);
        if (1UL << cur_index & blockers)
        {
            break;
        }
    }
    for (row = x+1, file=y-1; row<=7 && file>=0; ++row, --file)
    {
        cur_index = GetIndex(row, file);
        attacks |= (1UL << cur_index);
        if (1UL << cur_index & blockers)
        {
            break;
        }
    }
    for (row = x-1, file=y-1; row>=0 && file>=0; --row, --file)
    {
        cur_index = GetIndex(row, file);
        attacks |= (1UL << cur_index);
        if (1UL << cur_index & blockers)
        {
            break;
        }
    }

    return attacks;
}


/* generate rook possible moves on current board with blocking pieces */
bitmap_t GenerateRookAttacksOnTheFly(int square, bitmap_t blockers)
{
    bitmap_t board = 0L;

    int row = 0, file = 0, cur_index = 0;
    int x = 0, y = 0;

    x = GetRank(square);
    y = GetFile(square);

    for (row = x+1; row<=6; ++row)
    {
        cur_index = GetIndex(row, y);
        board |= (1UL << cur_index);
        if ((1UL << cur_index) & blockers)
        {
            break;
        }
    }
    for (row = x-1; row>=1; --row)
    {
        cur_index = GetIndex(row, y);
        board |= (1UL << cur_index);
        if ((1UL << cur_index) & blockers)
        {
            break;
        }
    }
    for (file=y-1; file>=1; --file)
    {
        cur_index = GetIndex(x, file);
        board |= (1UL << cur_index);
        if ((1UL << cur_index) & blockers)
        {
            break;
        }
    }
    for (file=y+1; file<=6; ++file)
    {
        cur_index = GetIndex(x, file);
        board |= (1UL << cur_index);
        if ((1UL << cur_index) & blockers)
        {
            break;
        }
    }

    return board;

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

/* init lut of pawns, knight attacks for every square */
void InitLeaperAttacks()
{
    int square = 0;

    for (; square<64; ++square)
    {
        PawnAttacks[square][WHITE] = GeneratePawnAttacks(square, WHITE);
        PawnAttacks[square][BLACK] = GeneratePawnAttacks(square, BLACK);
        KnightAttacks[square] = GenerateKnightAttacks(square);
        KingAttacks[square] = GenerateKingAttacks(square);
    }

}

/* getting occupancy map by atack mask */
/* index is number of occupancy variation out of all subsets of attack map*/
bitmap_t SetOccupancy(int index, int n_bits_in_mask, bitmap_t attack_mask)
{
    bitmap_t occupancy = 0L;
    int square = 0;
    int count = 0;


    for (count = 0;count < n_bits_in_mask; ++count)
    {
        square = GetLSBIndex(attack_mask);

        attack_mask =  BitBoardSetOff(attack_mask, square);
        if (index & (1 << count))
        {
            occupancy = BitBoardSetOn(occupancy, square);
        }
    }

    return occupancy;
}

/* find magic number for hashing for bishop and rook */
/* relevant bits - number of avaliable bits-squares for other figures */
bitmap_t FindMagicNumber(int square, int relevant_bits, int is_bishop)
{
    bitmap_t attack_mask = 0, magic_number = 0;

    /* 4096 - max number of possible occupancies for all squares of rook and bishop */
    bitmap_t occupancies[4096] = {0};
    bitmap_t attacks[4096] = {0};
    bitmap_t used_attacks[4096] = {0};

    int occupancy_indexes = 0, random_count = 0;

    int max_random_count = 100000000;

    int magic_index = 0, index = 0, fail = 0;

    attack_mask = is_bishop ? GenerateBishopAttacks(square) : GenerateRookAttacks(square);

    /* max number of possible occupancy blocking variations */
    occupancy_indexes = 1 << relevant_bits;

    for (index = 0; index < occupancy_indexes; ++index)
    {
        occupancies[index] = SetOccupancy(index, relevant_bits, attack_mask);
    
        attacks[index] = is_bishop ? GenerateBishopAttacksOnTheFly(square, occupancies[index]) : GenerateRookAttacksOnTheFly(square, occupancies[index]);
    }

    /* generate and test magic number, it should be perfect hash */
    for (random_count = 0; random_count < max_random_count; ++random_count)
    {
        magic_number = GenerateMagicNumber();
        /* skip wrong magic numbers */
        if (CountSetBits((attack_mask * magic_number) & 0xFF00000000000000) < 6)
        {
            continue;
        }
        
        /* set all used attacks to 0s s*/
        memset(used_attacks, 0UL, sizeof(used_attacks));

        for (index = 0,fail = 0; !fail && index < occupancy_indexes; ++index)
        {
            magic_index = (int)((occupancies[index]*magic_number) >> (64 - relevant_bits));
        
            if (used_attacks[magic_index] == 0UL)
            {
                used_attacks[magic_index] = attacks[index];
            }
            else if (used_attacks[magic_index] != attacks[index])
            {
                fail = 1;
            }
        }

        if (!fail)
        {
            return magic_number;
        }
    }

    puts("Magic number failed");

    return 0UL;
}

void InitMagicNumbers()
{
    int square = 0;
    bitmap_t magic_number = 0;
    /* for bishop */
    puts("Bishop magic numbers");
    for (square=0; square < 64; ++square)
    {
        magic_number = FindMagicNumber(square, BishopNMoves[square], 1);
        printf("0x%lxUL,\n", magic_number);
        BishopMagicNumbers[square] = magic_number;
    }

    puts("Rook magic numbers");
    for (square=0; square < 64; ++square)
    {
        
        magic_number = FindMagicNumber(square, BishopNMoves[square], 0);
        printf("0x%lxUL,\n", magic_number);
        RookMagicNumbers[square] = magic_number;
    }
}

void InitSliderAttacks()
{
    int square = 0;
    bitmap_t board = 0;

    for (; square<64; ++square)
    {
        board = GenerateBishopAttacks(square);
        BishopNMoves[square] = CountSetBits(board);
        board = GenerateRookAttacks(square);
        RookNMoves[square] = CountSetBits(board);
    }

}

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
    InitLeaperAttacks();

    /* init slider pieces - rook and bishop */
    /*InitSliderAttacks();*/

    /* init magic numbers */
    /*InitMagicNumbers();*/

    return board;
}

void DestroyBoard(board_t *board)
{
    free(board);
}