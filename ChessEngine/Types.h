#pragma once
#include <iostream>

using Bitmap = unsigned long long;
using uLong = unsigned long long;
using uInt = unsigned int;
using uShort = unsigned short;
using uChar = unsigned char;

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

inline constexpr uChar WHITE_KING_CASTLE = 1U; //= 0001
inline constexpr uChar WHITE_QUEEN_CASTLE = 2U; //= 0010
inline constexpr uChar BLACK_KING_CASTLE = 4U; //= 0100
inline constexpr uChar BLACK_QUEEN_CASTLE = 8U; //= 1000

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