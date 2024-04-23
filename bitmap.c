#include "bitmap.h" /* bitboard header */
#include <stdint.h> /* SIZE_MAX */


int BitBoardGetBit(bitmap_t bit_array,size_t index)
{
	return (bit_array >> index) & 1;
}

bitmap_t BitBoardSetBit(bitmap_t bit_array,size_t index, int val)
{
	return (bit_array & ~((size_t)1 << index)) | ((size_t)val << index);
}

bitmap_t BitBoardSetOff(bitmap_t bit_array,size_t index)
{
	return BitBoardSetBit(bit_array, index,  0);
}

bitmap_t BitBoardSetOn(bitmap_t bit_array,size_t index)
{
	return BitBoardSetBit(bit_array, index,  1);
}

bitmap_t BitBoardFlip(bitmap_t bit_array, size_t index)
{
	return (bit_array ^ ((size_t)1 << index));
}

int BitBoardIsOn(bitmap_t bit_array, size_t index)
{
	return BitBoardGetBit(bit_array, index);
}

int BitBoardCountSet(bitmap_t bit_array)
{
	size_t i = 0;
	int res = 0;
	
	for (i = 0; i < NUM_BITS; ++i)
	{
		res += BitBoardGetBit(bit_array, i);
	}
	
	return res;
}

int BitBoardCountOff(bitmap_t bit_array)
{
	return NUM_BITS - BitBoardCountSet(bit_array);
}


bitmap_t BitBoardSetAll(bitmap_t bit_array)
{
	UNUSED(bit_array);
	return SIZE_MAX; 
}
