#pragma once
#include "Types.h"

inline constexpr Bitmap SQUARE_MASK[] = {
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

inline constexpr char PIECE_CHAR[12] = { 'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k' };

inline constexpr int INFINIT = 30000;

inline constexpr int MATE = 29000;

inline constexpr uChar MAX_MOVES = 128U;

inline constexpr uChar MAX_DEPTH = 64U;

/* This function returns the File of Square */
inline File getFileOf(const Square square) noexcept {
	if (square >= NONE_SQUARE) return NONE_FILE;
	return static_cast<File>(square & 7U);
	// file = square % 8; or file = square & 7;
}
/* This function returns the Rank of Square */
inline Rank getRankOf(const Square square) noexcept {
	if (square >= NONE_SQUARE) return NONE_RANK;
	return static_cast<Rank>(square >> 3);
	// rank = square / 8; or rank = square >> 3;
}
/* This function returns the Square of File and Rank */
inline Square getSquareOf(const File file, const Rank rank) noexcept {
	if (file >= NONE_FILE || rank >= NONE_RANK) return NONE_SQUARE;
	return static_cast<Square>(rank << 3 | file);
	// square = 8 * rank + file; or square = (rank << 3) + file;
}

#if defined(__GNUC__) // GCC, Clang, ICC
/*
* all the GCC function must be tested!!!
* i am not sure if they are working
*/

/* This function returns the position(Square) from the left most set bit */
inline Square getLastSquareOf(const Bitmap bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i = __builtin_ctzll(bitmap);
	return static_cast<Square>(i);
}
/* This function returns and clear the position(Square) from the left most set bit */
inline Square popLastSquareOf(Bitmap& bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i = __builtin_ctzll(bitmap);
	bitmap &= ~(SQUARE_MASK << i);
	return static_cast<Square>(i);
}
/* This function returns the position(Square) from the right most set bit */
inline Square getFirstSquareOf(const Bitmap bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i = 63 ^ __builtin_clzll(bitmap);
	return static_cast<Square>(i);
}
/* This function returns and clear the position (Square) from the right most set bit */
inline Square popFirstSquareOf(Bitmap& bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i = 63 ^ __builtin_clzll(bitmap);
	bitmap &= (bitmap - 1U);
	return static_cast<Square>(i);
}
/* This function returns the number of set bits */
inline uChar bitCount(const Bitmap bitmap) noexcept {
	return static_cast<uChar>()__builtin_popcountll(bitmap));
}

#elif defined(_MSC_VER) // MSVC
#ifdef _WIN64 // MSVC, WIN64

/* This function returns the position(Square) from the left most set bit */
inline Square getLastSquareOf(const Bitmap bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanReverse64(&i, bitmap);
	return static_cast<Square>(i);
}
/* This function returns and clear the position(Square) from the left most set bit */
inline Square popLastSquareOf(Bitmap& bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanReverse64(&i, bitmap);
	bitmap &= ~(SQUARE_MASK[i]);
	return static_cast<Square>(i);
}
/* This function returns the position(Square) from the right most set bit */
inline Square getFirstSquareOf(const Bitmap bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanForward64(&i, bitmap);
	return static_cast<Square>(i);
}
/* This function returns and clear the position (Square) from the right most set bit */
inline Square popFirstSquareOf(Bitmap& bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanForward64(&i, bitmap);
	bitmap &= (bitmap - 1U);
	return static_cast<Square>(i);
}
/* This function returns the number of set bits */
inline uChar bitCount(const Bitmap bitmap) noexcept {
	return static_cast<uChar>(__popcnt64(bitmap));
}

#else // MSVC, WIN32
#error "Processor 32 bit is not compatible."
#endif
#else // Compiler is not compatible
#error "Compiler not supported."
#endif

/* This function returns the reverse bit position*/
inline Bitmap reverse(Bitmap bitmap) noexcept {
	bitmap = (bitmap & 0x5555555555555555UL) << 1 | (bitmap >> 1) & 0x5555555555555555UL;
	bitmap = (bitmap & 0x3333333333333333UL) << 2 | (bitmap >> 2) & 0x3333333333333333UL;
	bitmap = (bitmap & 0x0F0F0F0F0F0F0F0FUL) << 4 | (bitmap >> 4) & 0x0F0F0F0F0F0F0F0FUL;
	bitmap = (bitmap & 0x00FF00FF00FF00FFUL) << 8 | (bitmap >> 8) & 0x00FF00FF00FF00FFUL;
	bitmap = (bitmap << 48) | ((bitmap & 0xFFFF0000UL) << 16) |
		((bitmap >> 16) & 0xFFFF0000UL) | (bitmap >> 48);
	return bitmap;
}

inline std::ostream& operator<<(std::ostream& os, const File& file) {
	if (file < NONE_FILE)
		os << static_cast<uChar>(file + 97U);
	else
		os << "NONE FILE";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Rank& rank) {
	if (rank < NONE_RANK)
		os << static_cast<uChar>(rank + 49U);
	else
		os << "NONE RANK";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Square& square) {
	if (square < NONE_SQUARE) {
		const File file = getFileOf(square);
		const Rank rank = getRankOf(square);
		os << file << rank;
	} else
		os << "NONE SQUARE";
	return os;
}

template <typename T>
	requires std::integral<T>
inline void printBits(const char* title, const T t) {
	std::cout << title << std::endl;
	const uInt size = sizeof(T) * 8;

	for (uInt i = 0U, j = size - 1U; i < size; ++i, --j) {
		std::cout << (t >> j & 1);
		if (j % 8 == 0U)
			std::cout << ' ';
	}
	std::cout << std::endl;
}