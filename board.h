#ifndef __BOARD_H__
#define __BOARD_H__

#include "bitmap.h" /* bitboard header */
#include "random_numbers.h" /* rand numbers header */

#define N_SQUARES 8
#define OPOSITE_COLOR(color) (color == WHITE) ? BLACK : WHITE


/* all squares enumerated */
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

typedef enum 
{
    WHITE,
    BLACK,
    ALL
} color_t;

typedef struct color_board color_board_t;

typedef struct board board_t;



board_t *CreateBoard();

void DestroyBoard(board_t *board);

bitmap_t GeneratePawnAttacks(int square, color_t color);

#ifndef NDEBUG

extern bitmap_t PawnAttacks[N_SQUARES*N_SQUARES][ALL];
extern bitmap_t KnightAttacks[N_SQUARES*N_SQUARES];
extern bitmap_t KingAttacks[N_SQUARES*N_SQUARES];

extern int BishopNMoves[64];
extern int RookNMoves[64];


void PrintBoard(board_t *board);
void PrintColorBoard(board_t *board, color_t color);
void PrintBitsBoard(bitmap_t board);
bitmap_t SetOccupancy(int index, int n_bits_in_mask, bitmap_t attack_mask);
bitmap_t GenerateRookAttacks(int square);
bitmap_t GenerateBishopAttacks(int square);
int GetIndex(int rank, int file);
void InitSliderAttacks();
bitmap_t GenerateBishopAttacksOnTheFly(int square, bitmap_t blockers);
bitmap_t SetOccupancy(int index, int n_bits_in_mask, bitmap_t attack_mask);
void InitMagicNumbers();
bitmap_t GetBishopAttacks(int square, bitmap_t occupancy);
bitmap_t GetRookAttacks(int square, bitmap_t occupancy);
bitmap_t GetQueenAttacks(int square, bitmap_t occupancy);
#endif

#endif