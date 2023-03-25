#pragma once
#include "Types.h"

using BitBoard = unsigned long long;

inline const BitBoard FILES[8] = {
0x0101010101010101UL, 0x0202020202020202UL , 0x0404040404040404UL, 0x0808080808080808UL,
0x1010101010101010UL, 0x2020202020202020UL, 0x4040404040404040UL, 0x8080808080808080UL };

inline const BitBoard RANKS[8] = {
0x00000000000000FFUL, 0x000000000000FF00UL, 0x0000000000FF0000UL, 0x00000000FF000000UL,
0x000000FF00000000UL, 0x0000FF0000000000UL, 0x00FF000000000000UL, 0xFF00000000000000UL };

inline const BitBoard DIAGONAL[15] = {
0x0000000000000080UL, 0x0000000000008040UL, 0x0000000000804020UL, 0x0000000080402010UL,
0x0000008040201008UL, 0x0000804020100804UL, 0x0080402010080402UL, 0x8040201008040201UL,
0x4020100804020100UL, 0x2010080402010000UL, 0x1008040201000000UL, 0x0804020100000000UL,
0x0402010000000000UL, 0x0201000000000000UL, 0x0100000000000000UL };

inline const BitBoard ANTI_DIAGONAL[15] = {
0x0000000000000001UL, 0x0000000000000102UL, 0x0000000000010204UL, 0x0000000001020408UL,
0x0000000102040810UL, 0x0000010204081020UL, 0x0001020408102040UL, 0x0102040810204080UL,
0x0204081020408000UL, 0x0408102040800000UL, 0x0810204080000000UL, 0x1020408000000000UL,
0x2040800000000000UL, 0x4080000000000000UL, 0x8000000000000000UL };

inline const BitBoard SQUARE_MASK[] = {
0x1UL, 0x2UL, 0x4UL, 0x8UL,
0x10UL, 0x20UL, 0x40UL, 0x80UL,
0x100UL, 0x200UL, 0x400UL, 0x800UL,
0x1000UL, 0x2000UL, 0x4000UL, 0x8000UL,
0x10000UL, 0x20000UL, 0x40000UL, 0x80000UL,
0x100000UL, 0x200000UL, 0x400000UL, 0x800000UL,
0x1000000UL, 0x2000000UL, 0x4000000UL, 0x8000000UL,
0x10000000UL, 0x20000000UL, 0x40000000UL, 0x80000000UL,
0x100000000UL, 0x200000000UL, 0x400000000UL, 0x800000000UL,
0x1000000000UL, 0x2000000000UL, 0x4000000000UL, 0x8000000000UL,
0x10000000000UL, 0x20000000000UL, 0x40000000000UL, 0x80000000000UL,
0x100000000000UL, 0x200000000000UL, 0x400000000000UL, 0x800000000000UL,
0x1000000000000UL, 0x2000000000000UL, 0x4000000000000UL, 0x8000000000000UL,
0x10000000000000UL, 0x20000000000000UL, 0x40000000000000UL, 0x80000000000000UL,
0x100000000000000UL, 0x200000000000000UL, 0x400000000000000UL, 0x800000000000000UL,
0x1000000000000000UL, 0x2000000000000000UL, 0x4000000000000000UL, 0x8000000000000000UL,
0x0UL };

#if defined(__GNUC__) // GCC, Clang, ICC
/*
* all the GCC function must be tested!!!
* i am not sure if they are working
*/

/* This function returns the position(Square) from the left most set bit */
inline Square getLastSquareOf(const BitBoard bitBoard) noexcept {
	if (bitBoard == 0UL) return NONE_SQUARE;
	unsigned long i = __builtin_ctzll(bitBoard);
	return static_cast<Square>(i);
}

/* This function returns and clear the position(Square) from the left most set bit */
inline Square popLastSquareOf(BitBoard& bitBoard) noexcept {
	if (bitBoard == 0UL) return NONE_SQUARE;
	unsigned long i = __builtin_ctzll(bitBoard);
	bitmap &= ~(SQUARE_MASK << i);
	return static_cast<Square>(i);
}

/* This function returns the position(Square) from the right most set bit */
inline Square getFirstSquareOf(const BitBoard bitBoard) noexcept {
	if (bitBoard == 0UL) return NONE_SQUARE;
	unsigned long i = 63U ^ __builtin_clzll(bitBoard);
	return static_cast<Square>(i);
}

/* This function returns and clear the position (Square) from the right most set bit */
inline Square popFirstSquareOf(BitBoard& bitBoard) noexcept {
	if (bitBoard == 0UL) return NONE_SQUARE;
	unsigned long i = 63U ^ __builtin_clzll(bitBoard);
	bitmap &= (bitmap - 1U);
	return static_cast<Square>(i);
}

/* This function returns the number of set bits */
inline uChar bitCount(const BitBoard bitBoard) noexcept {
	return static_cast<uChar>()__builtin_popcountll(bitBoard));
}

#elif defined(_MSC_VER) // MSVC
#ifdef _WIN64 // MSVC, WIN64

/* This function returns the position(Square) from the left most set bit */
inline Square getLastSquareOf(const BitBoard bitBoard) noexcept {
	if (bitBoard == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanReverse64(&i, bitBoard);
	return static_cast<Square>(i);
}

/* This function returns and clear the position(Square) from the left most set bit */
inline Square popLastSquareOf(BitBoard& bitBoard) noexcept {
	if (bitBoard == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanReverse64(&i, bitBoard);
	bitBoard &= ~(SQUARE_MASK[i]);
	return static_cast<Square>(i);
}

/* This function returns the position(Square) from the right most set bit */
inline Square getFirstSquareOf(const BitBoard bitBoard) noexcept {
	if (bitBoard == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanForward64(&i, bitBoard);
	return static_cast<Square>(i);
}

/* This function returns and clear the position (Square) from the right most set bit */
inline Square popFirstSquareOf(BitBoard& bitBoard) noexcept {
	if (bitBoard == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanForward64(&i, bitBoard);
	bitBoard &= (bitBoard - 1U);
	return static_cast<Square>(i);
}

/* This function returns the number of set bits */
inline uChar bitCount(const BitBoard bitBoard) noexcept {
	return static_cast<uChar>(__popcnt64(bitBoard));
}

#else // MSVC, WIN32
#error "Processor 32 bit is not compatible."
#endif
#else // Compiler is not compatible
#error "Compiler not supported."
#endif

/* This function returns the reverse bit position */
inline BitBoard getReverse(BitBoard bitBoard) noexcept {
	bitBoard = (bitBoard & 0x5555555555555555UL) << 1 | (bitBoard >> 1) & 0x5555555555555555UL;
	bitBoard = (bitBoard & 0x3333333333333333UL) << 2 | (bitBoard >> 2) & 0x3333333333333333UL;
	bitBoard = (bitBoard & 0x0F0F0F0F0F0F0F0FUL) << 4 | (bitBoard >> 4) & 0x0F0F0F0F0F0F0F0FUL;
	bitBoard = (bitBoard & 0x00FF00FF00FF00FFUL) << 8 | (bitBoard >> 8) & 0x00FF00FF00FF00FFUL;
	bitBoard = (bitBoard << 48) | ((bitBoard & 0xFFFF0000UL) << 16) |
		((bitBoard >> 16) & 0xFFFF0000UL) | (bitBoard >> 48);
	return bitBoard;
}

inline void printBitBoard(const char* title, const BitBoard bitBoard) {
	std::cout << title << std::endl;
	for (int i = 56, j; i >= 0; i = i - 8) {
		for (j = 0; j < 8; j++)
			if ((bitBoard >> (i + j) & 1))// != 0
				std::cout << 1;
			else
				std::cout << 0;
		std::cout << std::endl;
	}
}