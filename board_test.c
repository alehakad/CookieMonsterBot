
#include <stddef.h> /* size_t */
#include <stdio.h> /* printf */
#include "board.h" /* board header */


void TestBoard()
{
    board_t *board = NULL;

    puts("Test board representation\n");
    board = CreateBoard();

    PrintBoard(board);

    DestroyBoard(board);

}

void TestPawnAttacks()
{
    board_t *board = NULL;

    puts("Test pawn attacks\n");
    board = CreateBoard();

    PrintBitsBoard(GeneratePawnAttacks(a4, WHITE));

    PrintBitsBoard(GeneratePawnAttacks(b4, WHITE));

    PrintBitsBoard(GeneratePawnAttacks(f2, BLACK));
    
    PrintBitsBoard(PawnAttacks[a4][WHITE]);

    DestroyBoard(board);
}

void TestKnightAttack()
{
    board_t *board = NULL;

    puts("Test knight attacks\n");
    board = CreateBoard();
    
    PrintBitsBoard(KnightAttacks[a4]);
    PrintBitsBoard(KnightAttacks[e5]);

    DestroyBoard(board);
}

void TestKingAttacks()
{

    board_t *board = NULL;

    puts("Test knight attacks\n");
    board = CreateBoard();
    
    PrintBitsBoard(KingAttacks[a4]);
    PrintBitsBoard(KingAttacks[e5]);

    DestroyBoard(board);
}

void TestOccupancySquares()
{
    board_t *board = NULL;
    bitmap_t rook_attack = 0;

    puts("Test occupancy squares\n");
    board = CreateBoard();
    
    rook_attack = GenerateRookAttacks(a4);
    PrintBitsBoard(SetOccupancy(3, CountSetBits(rook_attack), rook_attack));

    DestroyBoard(board);
}

int main()
{
    TestBoard();
    TestPawnAttacks();
    TestKnightAttack();
    TestKingAttacks();
    TestOccupancySquares();

    return 0;
}