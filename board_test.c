
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

    puts("Test king attacks\n");
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

void TestOnFlyAttacks()
{
    int square = 0, o_index = 0;
    bitmap_t board = 0, o_board = 0;
    InitSliderAttacks();
    for (square = 0; square < 64; ++square)
    {
        printf("square number %d\n", square);
        o_board = SetOccupancy(o_index, BishopNMoves[square], GenerateBishopAttacks(square));
        printf("occupancy board is \n");
        PrintBitsBoard(o_board);
        board = GenerateBishopAttacksOnTheFly(square,o_board);
        printf("bishop moves are  \n");
        PrintBitsBoard(board);
        
        getchar();
    }
}

void TestMagicNumbers()
{
    InitMagicNumbers();
}


int main()
{
    
    TestBoard();
    TestPawnAttacks();
    TestKnightAttack();
    TestKingAttacks();
    TestOccupancySquares();
    
    TestOnFlyAttacks();
    
    /* TestMagicNumbers(); */

    return 0;
}