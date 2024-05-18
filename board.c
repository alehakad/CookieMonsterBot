#include <stddef.h> /* size_t */
#include <stdlib.h> /* malloc */
#include <stdlib.h> /* rand */
#include <time.h> /* time */
#include <string.h> /* memset */

#ifndef NDEBUG
#include <stdio.h> /* printf */
#include <wchar.h> /* to print unicode */
#include <locale.h> /* locale */
#endif

#include "board.h" /* board header */

/* FEN positions to represent board */
/* 8 ranks, side_to_move, castling rights, enpassant squares */
/* numbers represent empty squares */
#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"

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
    int castle_k;
    int castle_q;
};

struct board
{
    color_board_t whites;
    color_board_t blacks;
    bitmap_t all;
    int side_to_move;
    int enpassant;
};

enum 
{
    ROOK,
    BISHOP
};


enum
{
    wp = L'♙',
    wkn = L'♘',
    wr = L'♖',
    wq = L'♕',
    wk = L'♔',
    wb = L'♗',

};

static bitmap_t ClearRankLUT[N_SQUARES];
static bitmap_t MaskRankLUT[N_SQUARES];
static bitmap_t ClearFileLUT[N_SQUARES];
static bitmap_t MaskFileLUT[N_SQUARES];
static bitmap_t PieceLUT[N_SQUARES*N_SQUARES];
bitmap_t PawnAttacks[N_SQUARES*N_SQUARES][ALL];
bitmap_t KnightAttacks[N_SQUARES*N_SQUARES];
bitmap_t KingAttacks[N_SQUARES*N_SQUARES];

bitmap_t BishopMasks[N_SQUARES*N_SQUARES];
bitmap_t RookMasks[N_SQUARES*N_SQUARES];

/* bishop and rook attacks by square and occupancy */
bitmap_t BishopAttacks[N_SQUARES*N_SQUARES][512];
bitmap_t RookAttacks[N_SQUARES*N_SQUARES][4096];


/* number of set bits from each position */
int BishopNMoves[64] = 
{
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    6, 5, 5, 5, 5, 5, 5, 6,
};
int RookNMoves[64] = 
{
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    12, 11, 11, 11, 11, 11, 11, 12,
};

/* magic numbers */
static bitmap_t BishopMagicNumbers[64] = 
{
    0x40040844404084UL,
    0x2004208a004208UL,
    0x10190041080202UL,
    0x108060845042010UL,
    0x581104180800210UL,
    0x2112080446200010UL,
    0x1080820820060210UL,
    0x3c0808410220200UL,
    0x4050404440404UL,
    0x21001420088UL,
    0x24d0080801082102UL,
    0x1020a0a020400UL,
    0x40308200402UL,
    0x4011002100800UL,
    0x401484104104005UL,
    0x801010402020200UL,
    0x400210c3880100UL,
    0x404022024108200UL,
    0x810018200204102UL,
    0x4002801a02003UL,
    0x85040820080400UL,
    0x810102c808880400UL,
    0xe900410884800UL,
    0x8002020480840102UL,
    0x220200865090201UL,
    0x2010100a02021202UL,
    0x152048408022401UL,
    0x20080002081110UL,
    0x4001001021004000UL,
    0x800040400a011002UL,
    0xe4004081011002UL,
    0x1c004001012080UL,
    0x8004200962a00220UL,
    0x8422100208500202UL,
    0x2000402200300c08UL,
    0x8646020080080080UL,
    0x80020a0200100808UL,
    0x2010004880111000UL,
    0x623000a080011400UL,
    0x42008c0340209202UL,
    0x209188240001000UL,
    0x400408a884001800UL,
    0x110400a6080400UL,
    0x1840060a44020800UL,
    0x90080104000041UL,
    0x201011000808101UL,
    0x1a2208080504f080UL,
    0x8012020600211212UL,
    0x500861011240000UL,
    0x180806108200800UL,
    0x4000020e01040044UL,
    0x300000261044000aUL,
    0x802241102020002UL,
    0x20906061210001UL,
    0x5a84841004010310UL,
    0x4010801011c04UL,
    0xa010109502200UL,
    0x4a02012000UL,
    0x500201010098b028UL,
    0x8040002811040900UL,
    0x28000010020204UL,
    0x6000020202d0240UL,
    0x8918844842082200UL,
    0x4010011029020020UL
};

static bitmap_t RookMagicNumbers[64] = 
{
    0x8a80104000800020UL,
    0x140002000100040UL,
    0x2801880a0017001UL,
    0x100081001000420UL,
    0x200020010080420UL,
    0x3001c0002010008UL,
    0x8480008002000100UL,
    0x2080088004402900UL,
    0x800098204000UL,
    0x2024401000200040UL,
    0x100802000801000UL,
    0x120800800801000UL,
    0x208808088000400UL,
    0x2802200800400UL,
    0x2200800100020080UL,
    0x801000060821100UL,
    0x80044006422000UL,
    0x100808020004000UL,
    0x12108a0010204200UL,
    0x140848010000802UL,
    0x481828014002800UL,
    0x8094004002004100UL,
    0x4010040010010802UL,
    0x20008806104UL,
    0x100400080208000UL,
    0x2040002120081000UL,
    0x21200680100081UL,
    0x20100080080080UL,
    0x2000a00200410UL,
    0x20080800400UL,
    0x80088400100102UL,
    0x80004600042881UL,
    0x4040008040800020UL,
    0x440003000200801UL,
    0x4200011004500UL,
    0x188020010100100UL,
    0x14800401802800UL,
    0x2080040080800200UL,
    0x124080204001001UL,
    0x200046502000484UL,
    0x480400080088020UL,
    0x1000422010034000UL,
    0x30200100110040UL,
    0x100021010009UL,
    0x2002080100110004UL,
    0x202008004008002UL,
    0x20020004010100UL,
    0x2048440040820001UL,
    0x101002200408200UL,
    0x40802000401080UL,
    0x4008142004410100UL,
    0x2060820c0120200UL,
    0x1001004080100UL,
    0x20c020080040080UL,
    0x2935610830022400UL,
    0x44440041009200UL,
    0x280001040802101UL,
    0x2100190040002085UL,
    0x80c0084100102001UL,
    0x4024081001000421UL,
    0x20030a0244872UL,
    0x12001008414402UL,
    0x2006104900a0804UL,
    0x1004081002402UL
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

    for (row = x+1; row<=7; ++row)
    {
        cur_index = GetIndex(row, y);
        board |= (1UL << cur_index);
        if ((1UL << cur_index) & blockers)
        {
            break;
        }
    }
    for (row = x-1; row>=0; --row)
    {
        cur_index = GetIndex(row, y);
        board |= (1UL << cur_index);
        if ((1UL << cur_index) & blockers)
        {
            break;
        }
    }
    for (file=y-1; file>=0; --file)
    {
        cur_index = GetIndex(x, file);
        board |= (1UL << cur_index);
        if ((1UL << cur_index) & blockers)
        {
            break;
        }
    }
    for (file=y+1; file<=7; ++file)
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

static wchar_t GetFigure(board_t *board, color_t color, int square)
{
    color_board_t c_board = (color == WHITE) ? board->whites : board->blacks;
    int offset = (color == WHITE) ? 0 : 6; 
    
    if (BitBoardIsOn(c_board.pawns, square))
    {
        return wp+offset;
    }
    if (BitBoardIsOn(c_board.knights, square))
    {
        return wk+offset;
    }
    if (BitBoardIsOn(c_board.rooks, square))
    {
        return wr+offset;
    }
    if (BitBoardIsOn(c_board.queen, square))
    {
        return wq+offset;
    }
    if (BitBoardIsOn(c_board.king, square))
    {
        return wk+offset;
    }
    if (BitBoardIsOn(c_board.bishops, square))
    {
        return wb+offset;
    }
    if (BitBoardIsOn(c_board.all, square))
    {
        return '1';
    }

    return '.';
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
    wchar_t figure = 0;

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
            if (color == ALL && GetFigure(board, color, square) == '.')
            {
                figure = GetFigure(board, OPOSITE_COLOR(color), square);
            }
            printf("%lc ",figure);
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
    printf("Side: %s\n", board->side_to_move ? "black" : "white");
    printf("Enpassant: %s\n", (board->enpassant != no_sq) ? "yes" : "no");
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

void InitSliderAttacks(int is_bishop)
{
    int square = 0;
    bitmap_t attack_mask = 0, occupancy = 0;
    int n_bits = 0, n_occupancies = 0, index = 0, magic_index = 0;

    for (; square<64; ++square)
    {
        if (is_bishop)
        {
            attack_mask = GenerateBishopAttacks(square);
            /* BishopNMoves[square] = CountSetBits(attack_mask); */
            BishopMasks[square] = attack_mask;
        }
        else
        {

            attack_mask = GenerateRookAttacks(square);
            /* RookNMoves[square] = CountSetBits(attack_mask); */
            RookMasks[square] = attack_mask;
        }

        n_bits = CountSetBits(attack_mask);

        n_occupancies = (1 << n_bits);

        for (index=0; index<n_occupancies; ++index)
        {
            occupancy = SetOccupancy(index, n_bits, attack_mask);

            if (is_bishop)
            {
                magic_index = (occupancy * BishopMagicNumbers[square]) >> (64 - BishopNMoves[square]); 
                BishopAttacks[square][magic_index] = GenerateBishopAttacksOnTheFly(square, occupancy);
            }
            else
            {
                magic_index = (occupancy * RookMagicNumbers[square]) >> (64 - RookNMoves[square]); 
                RookAttacks[square][magic_index] = GenerateRookAttacksOnTheFly(square, occupancy);
            }
        
        
        }
    
    }  

}

/* get bishop attacks for current occupancy */
bitmap_t GetBishopAttacks(int square, bitmap_t occupancy)
{
    occupancy &= BishopMasks[square];
    occupancy *= BishopMagicNumbers[square];
    occupancy >>= (64 - BishopNMoves[square]);

    return BishopAttacks[square][occupancy];
}

/* get rook attacks for current occupancy */
bitmap_t GetRookAttacks(int square, bitmap_t occupancy)
{
    occupancy &= RookMasks[square];
    occupancy *= RookMagicNumbers[square];
    occupancy >>= (64 - RookNMoves[square]);

    return RookAttacks[square][occupancy];
}


/* get queen attacks for current occupancy by combaining rook and bishop attacks */
bitmap_t GetQueenAttacks(int square, bitmap_t occupancy)
{
    bitmap_t queen_attacks = 0;

    queen_attacks = GetBishopAttacks(square, occupancy);

    queen_attacks |= GetRookAttacks(square, occupancy);

    return queen_attacks;
}

board_t *CreateBoard()
{
    board_t *board = NULL;  

    setlocale(LC_CTYPE, "en_US.UTF-8");

    board = malloc(sizeof(board_t));
    if (!board)
    {
        return board;
    }
    
    board->whites = FillColorBoard(WHITE);
    board->blacks = FillColorBoard(BLACK);
    board->all = board->whites.all | board->blacks.all;

    board->enpassant = no_sq;
    board->side_to_move = 0;
    board->whites.castle_k = 1;
    board->whites.castle_q = 1;
    board->blacks.castle_k = 1;
    board->blacks.castle_q = 1;

    /* init luts for files and ranks */
    FillLuts();

    /* init attacks luts for pieces */
    InitLeaperAttacks();

    /* init slider pieces - rook and bishop */
    InitSliderAttacks(BISHOP);
    InitSliderAttacks(ROOK);


    /* init magic numbers */
    /*InitMagicNumbers();*/


    return board;
}

void DestroyBoard(board_t *board)
{
    free(board);
}


/* parse FEN string */
void parse_fen(char *fen)
{
    /* TODO: */
}