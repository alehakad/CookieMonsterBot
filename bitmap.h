#ifndef __BITBOARD_H__
#define __BITBOARD_H__

#include <stddef.h> /* size_t */

#define UNUSED(x) (void)x
#define NUM_BITS 64
#define BITS_IN_BYTE 8



typedef size_t bitmap_t;



/*
Description:
Get the value of the bit in the index.

Arguments:
	bit_array.
	index: bit index , values between 0 - 63.

Return value: 0 or 1.

*/
int BitBoardGetBit(bitmap_t bit_array,size_t index); 


/*
Description:
Setting the bit at the index.

Arguments:
	bit_array.
	index: bit index , values between 0 - 63.
	val: value 0 or 1.

Return value: A bit array with the index set as requested.

*/
bitmap_t BitBoardSetBit(bitmap_t bit_array,size_t index, int val); 


/*
Description:
Resets bit at the index to 0.

Arguments:
	bit_array.
	index: bit index , values between 0 - 63.

Return value: A bit array with the index set as requested.

*/
bitmap_t BitBoardSetOff(bitmap_t bit_array,size_t index);


/*
Description:
Resets bit at the index to 1.

Arguments:
	bit_array.
	index: bit index , values between 0 - 63.

Return value: A bit array with the index set as requested.

*/
bitmap_t BitBoardSetOn(bitmap_t bit_array,size_t index);


/*
Description:
Flips the bit at index , 0 to 1 , 1 to 0.

Arguments:
	bit_array.
	index: bit index , values between 0 - 63.

Return value: A bit array with the index set as requested.

*/
bitmap_t BitBoardFlip(bitmap_t bit_array, size_t index);

/*
Description:
Checks if the bit is on.

Arguments:
	bit_array.
	index: bit index , values between 0 - 63.

Return value: 1 if the bit was off , otherwise 0

*/
int BitBoardIsOn(bitmap_t bit_array, size_t index);

/*
Description:
Counts the amount of set bits in a given bit array.

Arguments:
	bit_array.

Return value: amount of set bits , 0 - 63.

*/
int BitBoardCountSet(bitmap_t bit_array);

/*
Description:
Counts the amount of off bits in a given bit array.

Arguments:
	bit_array.

Return value: amount of off bits , 0 - 63.

*/
int BitBoardCountOff(bitmap_t bit_array);

/*
Description:
Set all bits in bit array

Arguments:
	bit_array.

Return value: max possible bit array with all bits set.

*/
bitmap_t BitBoardSetAll(bitmap_t bit_array);

/*
Description:
Count set bits in bit array with Kernigan's algorithm

Arguments:
	bit_array.

Return value: number of set bits.

*/
int CountSetBits(bitmap_t bit_array);



/*
Description:
Get index of LSB of bit array

Arguments:
	bit_array.

Return value: index of first set bit in bit_array.

*/
int GetLSBIndex(bitmap_t bit_array);


#endif