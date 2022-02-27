#include "Types.h"

File& operator++(File& f) noexcept { return f = static_cast<File>((static_cast<uInt>(f) + 1U) % 9U); }
File& operator--(File& f) noexcept { return f = f == FILE_A ? NONE_FILE : static_cast<File>(static_cast<uInt>(f) - 1U); }
File operator~(const File& f) noexcept { return static_cast<File>(f ^ FILE_H); }

Rank& operator++(Rank& r) noexcept { return r = static_cast<Rank>((static_cast<uInt>(r) + 1U) % 9U); }
Rank& operator--(Rank& r) noexcept { return r = r == RANK_1 ? NONE_RANK : static_cast<Rank>(static_cast<uInt>(r) - 1U); }
Rank operator~(const Rank& r) noexcept { return static_cast<Rank>(r ^ RANK_8); }

Square& operator++(Square& s) noexcept { return s = s + 1U; }
Square& operator--(Square& s) noexcept { return s = s - 1U; }
Square operator~(const Square& s) noexcept { return static_cast<Square>(s ^ A8); }

Piece& operator++(Piece& p) noexcept { return p = static_cast<Piece>((static_cast<uChar>(p) + 1U) % 7U); }
Piece& operator--(Piece& p) noexcept { return p = p == PAWN ? NONE_PIECE : static_cast<Piece>(static_cast<uInt>(p) - 1U); }

Color operator~(const Color& color) noexcept { return static_cast<Color>(color ^ BLACK); }

const File getFileOf(const Square square) noexcept {
	if (square == NONE_SQUARE)
		return NONE_FILE;
	return static_cast<File>(square & 7U);
	// file = square % 8; or file = square & 7;
}

const Rank getRankOf(const Square square) noexcept {
	if (square == NONE_SQUARE)
		return NONE_RANK;
	return static_cast<Rank>(square >> 3);
	// rank = square / 8; or rank = square >> 3;
}

const Square getSquareOf(const File file, const Rank rank) noexcept {
	if (file == NONE_FILE || rank == NONE_RANK)
		return NONE_SQUARE;
	return static_cast<Square>((rank << 3) + file);
	// square = 8 * rank + file;
}

const Bitmap getBitmapOf(const Square square) noexcept {
	return SQUARE_MASK << square;
}

const Square getLastSquareOf(const Bitmap bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanReverse64(&i, bitmap);
	return static_cast<Square>(i);
}

const Square popLastSquareOf(Bitmap& bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanReverse64(&i, bitmap);
	bitmap &= ~(SQUARE_MASK << i);
	return static_cast<Square>(i);
}

const Square getFirstSquareOf(const Bitmap bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanForward64(&i, bitmap);
	return static_cast<Square>(i);
}

const Square popFirstSquareOf(Bitmap& bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanForward64(&i, bitmap);
	bitmap &= (bitmap - 1U);
	return static_cast<Square>(i);
}

std::ostream& operator<<(std::ostream& os, const File& file) {
	if (file < NONE_FILE)
		os << static_cast<uChar>(file + 97U);
	else
		os << "NONE FILE";
	return os;
}

std::ostream& operator<<(std::ostream& os, const Rank& rank) {
	if (rank < NONE_RANK)
		os << static_cast<uChar>(rank + 49U);
	else
		os << "NONE RANK";
	return os;
}


std::ostream& operator<<(std::ostream& os, const Square& square) {
	if (square < NONE_SQUARE) {
		const File file = getFileOf(square);
		const Rank rank = getRankOf(square);
		os << file << rank;
	}
	else
		os << "NONE SQUARE";
	return os;
}

const uInt bitCount(Bitmap bitmap) noexcept {
	bitmap = ((bitmap >> 1) & 0x5555555555555555UL) + (bitmap & 0x5555555555555555UL);
	bitmap = ((bitmap >> 2) & 0x3333333333333333UL) + (bitmap & 0x3333333333333333UL);
	uInt v = static_cast<uInt>((bitmap >> 32) + bitmap);
	v = ((v >> 4) & 0x0F0F0F0FU) + (v & 0x0F0F0F0FU);
	v = ((v >> 8) & 0x00FF00FFU) + (v & 0x00F00FFU);
	return ((v >> 16) & 0x0000FFFFU) + (v & 0x0000FFFFU);
}

const Bitmap reverse(Bitmap bitmap) noexcept {
	bitmap = (bitmap & 0x5555555555555555UL) << 1 | (bitmap >> 1) & 0x5555555555555555UL;
	bitmap = (bitmap & 0x3333333333333333UL) << 2 | (bitmap >> 2) & 0x3333333333333333UL;
	bitmap = (bitmap & 0x0F0F0F0F0F0F0F0FUL) << 4 | (bitmap >> 4) & 0x0F0F0F0F0F0F0F0FUL;
	bitmap = (bitmap & 0x00FF00FF00FF00FFUL) << 8 | (bitmap >> 8) & 0x00FF00FF00FF00FFUL;
	bitmap = (bitmap << 48) | ((bitmap & 0xFFFF0000UL) << 16) |
		((bitmap >> 16) & 0xFFFF0000UL) | (bitmap >> 48);
	return bitmap;
}