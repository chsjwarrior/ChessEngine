#pragma once
#include <iostream>
#include <algorithm>

using uLong = unsigned long long;
using uInt = unsigned int;
using uShort = unsigned short;
using uChar = unsigned char;

inline const char PIECE_CHAR[12] = { 'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k' };

inline const int INFINIT = 30000;

inline const int MATE = 29000;

inline const uShort MAX_MOVES = 256U;

inline const uChar MAX_DEPTH = 64U;

inline const uChar WHITE_KING_CASTLE = 1U; // = 0001

inline const uChar WHITE_QUEEN_CASTLE = 2U; // = 0010

inline const uChar BLACK_KING_CASTLE = 4U; // = 0100

inline const uChar BLACK_QUEEN_CASTLE = 8U; // = 1000

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
	NONE_SQUARE // = 0X40U = 64
};

enum Color : uChar { WHITE, BLACK };

enum Piece : uChar { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE_PIECE }; // NONE_PIECE = 0X6U

inline File& operator++(File& f) noexcept {
	return f = static_cast<File>((static_cast<uInt>(f) + 1U) % 9U);
}

inline File& operator--(File& f) noexcept {
	return f = f == FILE_A ? NONE_FILE : static_cast<File>(static_cast<uInt>(f) - 1U);
}

inline File operator~(const File& f) noexcept {
	return static_cast<File>(f ^ FILE_H);
}

inline Rank& operator++(Rank& r) noexcept {
	return r = static_cast<Rank>((static_cast<uInt>(r) + 1U) % 9U);
}

inline Rank& operator--(Rank& r) noexcept {
	return r = r == RANK_1 ? NONE_RANK : static_cast<Rank>(static_cast<uInt>(r) - 1U);
}

inline Rank operator~(const Rank& r) noexcept {
	return static_cast<Rank>(r ^ RANK_8);
}

template <typename T>
	requires std::integral<T>
inline Square operator+(Square s, const T t) noexcept {
	return static_cast<Square>((static_cast<T>(s) + t) % 65U);
}

template <typename T>
	requires std::integral<T>
inline Square operator-(Square s, const T t) noexcept {
	return static_cast<Square>((static_cast<T>(s) - (t % 64U)) % 64U);
}

inline Square& operator++(Square& s) noexcept {
	return s = s + 1U;
}

inline Square& operator--(Square& s) noexcept {
	return s = s - 1U;
}

inline Square operator~(const Square& s) noexcept {
	return static_cast<Square>(s ^ A8);
}

inline Piece& operator++(Piece& p) noexcept {
	return p = static_cast<Piece>((static_cast<uChar>(p) + 1U) % 7U);
}

inline Piece& operator--(Piece& p) noexcept {
	return p = p == PAWN ? NONE_PIECE : static_cast<Piece>(static_cast<uInt>(p) - 1U);
}

inline Color operator~(const Color& color) noexcept {
	return static_cast<Color>(color ^ BLACK);
}

/* This function return the relative Rank of color */
inline Rank getRelativeRankOf(const Color color, const Rank rank) noexcept {
	return color == WHITE ? rank : ~rank;
}

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

/* This function returns the Square of File and Rank intersection */
inline Square getSquareOf(const File file, const Rank rank) noexcept {
	if (file >= NONE_FILE || rank >= NONE_RANK) return NONE_SQUARE;
	return static_cast<Square>(rank << 3 | file);
	// square = 8 * rank + file; or square = (rank << 3) + file;
}
/* This function return the index of the diagonal from intersection of File and Rank */
inline uChar getDiagonalOf(const File file, const Rank rank) noexcept {
	return rank + 7 - file;
}

/* This function return the index of the diagonal from square */
inline uChar getDiagonalOf(const Square square) noexcept {
	return getDiagonalOf(getFileOf(square), getRankOf(square));
}

/* This function return the index of the anti-diagonal from intersection of File and Rank */
inline uChar getAntiDiagonalOf(const File file, const Rank rank) noexcept {
	return rank + file;
}

/* This function return the index of the anti-diagonal from square */
inline uChar getAntiDiagonalOf(const Square square) noexcept {
	return getAntiDiagonalOf(getFileOf(square), getRankOf(square));
}

/* this function return true if two squares are in same diagonal */
inline bool squaresOnSameDiagonal(const Square square1, const Square square2) noexcept {
	return ((square2 - square1) % 9) == 0;// if the square difference is divisible by 9 
	// return ((square2 - square1) & 7) == ((square2 >> 3) - (square1 >> 3));// if their file distance equals the rank distance
}

/* this function return true if two squares are in same anti-diagonal */
inline bool squaresOnSameAntiDiagonal(const Square square1, const Square square2) noexcept {
	return ((square2 - square1) % 7) == 0;// if the square difference is divisible by 7 
	// return ((sq2 - sq1) & 7) + ((sq2>>3) - (sq1>>3)) == 0;// if sum of file and rank distance is zero
}

/* this function return true if two squares are in same File */
inline bool squaresOnSameFile(const Square square1, const Square square2) {
	return ((square1 ^ square2) & 7) == 0;// The Symmetric difference\xor is zero
	// return ((square2 - square1) & 7) == 0;// if their file distance is zero.
}

/* this function return true if two squares are in same Rank */
inline bool squaresOnSameRank(const Square square1, const Square square2) {
	return ((square1 ^ square2) & 56) == 0;// The Symmetric difference\xor is zero
	// return ((square2 >> 3) - (square1 >> 3)) == 0;// if their Rank distance is zero
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