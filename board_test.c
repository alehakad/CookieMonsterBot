
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


void TestGenerateNSliderMoves(int is_bishop)
{
    int rank = 0, file = 0, res = 0, square = 0;

    for (rank = 0; rank<8; ++rank)
    {
        for (file = 0; file<8; ++file)
        {
            square = GetIndex(rank, file);
            if (is_bishop)
            {
                res = CountSetBits(GenerateBishopAttacks(square));
            }
            else
            {
                res = CountSetBits(GenerateRookAttacks(square));
            }
            printf("%d, ", res);

        }
        puts("");
    }
}

void TestBishopRookAttacks()
{
    board_t *board = NULL;
    bitmap_t bishop_attack = 0;

    board = CreateBoard();
    puts("Test bishop and rook attacks\n");
    
    puts("Print bishop attack");
    PrintBitsBoard(GetBishopAttacks(d4, bishop_attack));
    puts("Print rook attack");
    PrintBitsBoard(GetRookAttacks(d4, bishop_attack));

    bishop_attack = BitBoardSetOn(bishop_attack, c5);

    puts("Print bishop attack");
    PrintBitsBoard(GetBishopAttacks(d4, bishop_attack));

    bishop_attack = BitBoardSetOn(bishop_attack, f2);
    bishop_attack = BitBoardSetOn(bishop_attack, h1);
    bishop_attack = BitBoardSetOn(bishop_attack, d7);
    bishop_attack = BitBoardSetOn(bishop_attack, d1);
    bishop_attack = BitBoardSetOn(bishop_attack, f4);

    puts("Print bishop attack");
    PrintBitsBoard(GetBishopAttacks(d4, bishop_attack));
    puts("Print rook attack");
    PrintBitsBoard(GetRookAttacks(d4, bishop_attack));

    DestroyBoard(board);
}

void TestQueenAttacks()
{

    board_t *board = NULL;
    bitmap_t queen_attack = 0;

    board = CreateBoard();
    puts("Test queen attacks\n");

    puts("Print queen attack");
    PrintBitsBoard(GetQueenAttacks(d4, queen_attack));

    queen_attack = BitBoardSetOn(queen_attack, c1);
    puts("Print bishop attack");
    PrintBitsBoard(GetQueenAttacks(a1, queen_attack));

    DestroyBoard(board);
}


int main()
{
    
    TestBoard();
    /*
    TestPawnAttacks();
    TestKnightAttack();
    TestKingAttacks();
    TestOccupancySquares();
    */
    /* TestOnFlyAttacks(); */

    TestBishopRookAttacks();
    TestQueenAttacks();

    
    
    /* TestGenerateNSliderMoves(1); */
    /* TestGenerateNSliderMoves(0); */
    /* TestMagicNumbers(); */

    return 0;
}