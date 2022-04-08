#pragma once
#include <iostream>

using Bitmap = unsigned long long;
using uLong = unsigned long long;
using uInt = unsigned int;
using uShort = unsigned short;
using uChar = unsigned char;

inline constexpr char NAME[] = "ChessEngine 1.0";

inline constexpr char AUTHOR[] = "Carlos Henrique Stapait Junior";

inline constexpr char START_FEN[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

inline constexpr char PIECE_CHAR[6][2] = { {'P','p'}, {'N','n'}, {'B','b'}, {'R','r'}, {'Q','q'}, {'K','k'} };

inline constexpr int PIECE_VALUE[] = { 10,30,30,50,90,900,0 };

inline constexpr int INFINIT = 30000;

inline constexpr Bitmap SQUARE_MASK = 0x1UL;

inline constexpr uChar MAX_MOVES = 128U;

inline constexpr uChar MAX_DEPTH = 64U;

inline constexpr int SQUARE_VALUE[6][64] = {
		{//PAWN
			0,  0,  0,  0,  0,  0,  0,  0,
			50, 50, 50, 50, 50, 50, 50, 50,
			10, 10, 20, 30, 30, 20, 10, 10,
			5,  5, 10, 25, 25, 10,  5,  5,
			0,  0,  0, 20, 20,  0,  0,  0,
			5, -5,-10,  0,  0,-10, -5,  5,
			5, 10, 10,-20,-20, 10, 10,  5,
			0,  0,  0,  0,  0,  0,  0,  0},
		{//kNIGHT
			-50,-40,-30,-30,-30,-30,-40,-50,
			-40,-20,  0,  0,  0,  0,-20,-40,
			-30,  0, 10, 15, 15, 10,  0,-30,
			-30,  5, 15, 20, 20, 15,  5,-30,
			-30,  0, 15, 20, 20, 15,  0,-30,
			-30,  5, 10, 15, 15, 10,  5,-30,
			-40,-20,  0,  5,  5,  0,-20,-40,
			-50,-40,-30,-30,-30,-30,-40,-50},
		{//BISHOP
			-20,-10,-10,-10,-10,-10,-10,-20,
			-10,  0,  0,  0,  0,  0,  0,-10,
			-10,  0,  5, 10, 10,  5,  0,-10,
			-10,  5,  5, 10, 10,  5,  5,-10,
			-10,  0, 10, 10, 10, 10,  0,-10,
			-10, 10, 10, 10, 10, 10, 10,-10,
			-10,  5,  0,  0,  0,  0,  5,-10,
			-20,-10,-10,-10,-10,-10,-10,-20},
		{//ROOK
			 0,  0,  0,  0,  0,  0,  0,  0,
			 5, 10, 10, 10, 10, 10, 10,  5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			 0,  0,  0,  5,  5,  0,  0,  0},
		{//QUEEN
			-20,-10,-10, -5, -5,-10,-10,-20,
			-10,  0,  0,  0,  0,  0,  0,-10,
			-10,  0,  5,  5,  5,  5,  0,-10,
			-5,  0,  5,  5,  5,  5,  0, -5,
			 0,  0,  5,  5,  5,  5,  0, -5,
			-10,  5,  5,  5,  5,  5,  0,-10,
			-10,  0,  5,  0,  0,  0,  0,-10,
			-20,-10,-10, -5, -5,-10,-10,-20},
		{//KING
			-30,-40,-40,-50,-50,-40,-40,-30,
			-30,-40,-40,-50,-50,-40,-40,-30,
			-30,-40,-40,-50,-50,-40,-40,-30,
			-30,-40,-40,-50,-50,-40,-40,-30,
			-20,-30,-30,-40,-40,-30,-30,-20,
			-10,-20,-20,-20,-20,-20,-20,-10,
			 20, 20,  0,  0,  0,  0, 20, 20,
			 20, 30, 10,  0,  0, 10, 30, 20} };

enum CastleFlags : uChar { KING_CASTLE = 0, QUEEN_CASTLE = 2 };//for black (KING_CASTLE + color) = 1 (QUEEN_CASTLE + color) = 3

enum File : uChar { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, NONE_FILE };

enum Rank : uChar { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, NONE_RANK };

enum Square : uChar {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
	NONE_SQUARE //= 0X40U = 64
};

enum Color : uChar { WHITE, BLACK };

enum Piece : uChar { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE_PIECE }; // NONE_PIECE = 0X6U

constexpr File& operator++(File& f) noexcept { return f = static_cast<File>((static_cast<uInt>(f) + 1U) % 9U); }
constexpr File& operator--(File& f) noexcept { return f = f == FILE_A ? NONE_FILE : static_cast<File>(static_cast<uInt>(f) - 1U); }
constexpr File operator~(const File& f) noexcept { return static_cast<File>(f ^ FILE_H); }

constexpr Rank& operator++(Rank& r) noexcept { return r = static_cast<Rank>((static_cast<uInt>(r) + 1U) % 9U); }
constexpr Rank& operator--(Rank& r) noexcept { return r = r == RANK_1 ? NONE_RANK : static_cast<Rank>(static_cast<uInt>(r) - 1U); }
constexpr Rank operator~(const Rank& r) noexcept { return static_cast<Rank>(r ^ RANK_8); }

template <typename T>
	requires std::integral<T>
constexpr Square operator+(Square s, const T t) noexcept { return static_cast<Square>((static_cast<T>(s) + t) % 65U); }
template <typename T>
	requires std::integral<T>
constexpr Square operator-(Square s, const T t) noexcept { return static_cast<Square>((static_cast<T>(s) - (t % 64U)) % 64U); }
constexpr Square& operator++(Square& s) noexcept { return s = s + 1U; }
constexpr Square& operator--(Square& s) noexcept { return s = s - 1U; }
constexpr Square operator~(const Square& s) noexcept { return static_cast<Square>(s ^ A8); }

constexpr Piece& operator++(Piece& p) noexcept { return p = static_cast<Piece>((static_cast<uChar>(p) + 1U) % 7U); }
constexpr Piece& operator--(Piece& p) noexcept { return p = p == PAWN ? NONE_PIECE : static_cast<Piece>(static_cast<uInt>(p) - 1U); }

constexpr Color operator~(const Color& color) noexcept { return static_cast<Color>(color ^ BLACK); }

/* This function returns the File of Square*/
constexpr File getFileOf(const Square square) noexcept {
	if (square >= NONE_SQUARE) return NONE_FILE;
	return static_cast<File>(square & 7U);
	// file = square % 8; or file = square & 7;
}
/* This function returns the Rank of Square*/
constexpr Rank getRankOf(const Square square) noexcept {
	if (square >= NONE_SQUARE) return NONE_RANK;
	return static_cast<Rank>(square >> 3);
	// rank = square / 8; or rank = square >> 3;
}
/* This function returns the Square of File and Rank*/
constexpr Square getSquareOf(const File file, const Rank rank) noexcept {
	if (file >= NONE_FILE || rank >= NONE_RANK) return NONE_SQUARE;
	return static_cast<Square>((rank << 3) + file);
	// square = 8 * rank + file;
}
/* This function returns a Bitmap of Square*/
constexpr Bitmap getBitmapOf(const Square square) noexcept {
	if (square >= NONE_SQUARE) return 0UL;
	return SQUARE_MASK << square;
}
/*this function returns the position(Square) of the left most set bit*/
constexpr Square getLastSquareOf(const Bitmap bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanReverse64(&i, bitmap);
	return static_cast<Square>(i);
}
/*this function returns and clear the position(Square) of the left most set bit*/
constexpr Square popLastSquareOf(Bitmap& bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanReverse64(&i, bitmap);
	bitmap &= ~(SQUARE_MASK << i);
	return static_cast<Square>(i);
}
/*this function returns the position(Square) of the right most set bit*/
constexpr Square getFirstSquareOf(const Bitmap bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanForward64(&i, bitmap);
	return static_cast<Square>(i);
}
/*this function returns and clear the position (Square) of the right most set bit*/
constexpr Square popFirstSquareOf(Bitmap& bitmap) noexcept {
	if (bitmap == 0UL) return NONE_SQUARE;
	unsigned long i;
	_BitScanForward64(&i, bitmap);
	bitmap &= (bitmap - 1U);
	return static_cast<Square>(i);
}

constexpr std::ostream& operator<<(std::ostream& os, const File& file) {
	if (file < NONE_FILE)
		os << static_cast<uChar>(file + 97U);
	else
		os << "NONE FILE";
	return os;
}

constexpr std::ostream& operator<<(std::ostream& os, const Rank& rank) {
	if (rank < NONE_RANK)
		os << static_cast<uChar>(rank + 49U);
	else
		os << "NONE RANK";
	return os;
}

constexpr std::ostream& operator<<(std::ostream& os, const Square& square) {
	if (square < NONE_SQUARE) {
		const File file = getFileOf(square);
		const Rank rank = getRankOf(square);
		os << file << rank;
	} else
		os << "NONE SQUARE";
	return os;
}

constexpr uInt bitCount(Bitmap bitmap) noexcept {
	bitmap = ((bitmap >> 1) & 0x5555555555555555UL) + (bitmap & 0x5555555555555555UL);
	bitmap = ((bitmap >> 2) & 0x3333333333333333UL) + (bitmap & 0x3333333333333333UL);
	uInt v = static_cast<uInt>((bitmap >> 32) + bitmap);
	v = ((v >> 4) & 0x0F0F0F0FU) + (v & 0x0F0F0F0FU);
	v = ((v >> 8) & 0x00FF00FFU) + (v & 0x00F00FFU);
	return ((v >> 16) & 0x0000FFFFU) + (v & 0x0000FFFFU);
}

constexpr Bitmap reverse(Bitmap bitmap) noexcept {
	bitmap = (bitmap & 0x5555555555555555UL) << 1 | (bitmap >> 1) & 0x5555555555555555UL;
	bitmap = (bitmap & 0x3333333333333333UL) << 2 | (bitmap >> 2) & 0x3333333333333333UL;
	bitmap = (bitmap & 0x0F0F0F0F0F0F0F0FUL) << 4 | (bitmap >> 4) & 0x0F0F0F0F0F0F0F0FUL;
	bitmap = (bitmap & 0x00FF00FF00FF00FFUL) << 8 | (bitmap >> 8) & 0x00FF00FF00FF00FFUL;
	bitmap = (bitmap << 48) | ((bitmap & 0xFFFF0000UL) << 16) |
		((bitmap >> 16) & 0xFFFF0000UL) | (bitmap >> 48);
	return bitmap;
}

template <typename T>
	requires std::integral<T>
constexpr void printBits(const char* title, const T t) {
	std::cout << title << std::endl;
	const uInt size = sizeof(T) * 8;

	for (uInt i = 0U, j = size - 1U; i < size; ++i, --j) {
		std::cout << (t >> j & 1);
		if (j % 8 == 0U)
			std::cout << ' ';
	}
	std::cout << std::endl;
}