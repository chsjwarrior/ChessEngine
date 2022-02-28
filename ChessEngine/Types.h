#pragma once
#include <iostream>

typedef unsigned long long int Bitmap;
typedef unsigned long long int uLong;
typedef unsigned int uInt;
typedef unsigned short int uShort;
typedef unsigned char uChar;

constexpr char NAME[] = "ChessEngine 1.0";

constexpr char AUTHOR[] = "Carlos Henrique Stapait Junior";

constexpr char START_FEN[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

constexpr char PIECE_CHAR[6][2] = { {'P','p'}, {'N','n'}, {'B','b'}, {'R','r'}, {'Q','q'}, {'K','k'} };

constexpr int PIECE_VALUE[7] = { 10,30,30,50,90,900,0 };

constexpr uInt MOVE_EMPTY = 0x00664040U;

constexpr uShort BITBOARD_FLAGS_EMPTY = 0X0040U;

constexpr Bitmap PATH_KING_CASTLE = 0x6000000000000060UL;

constexpr Bitmap PATH_QUEEN_CASTLE = 0x0E0000000000000EUL;

constexpr Bitmap SQUARE_MASK = 0x1UL;

constexpr uChar MAX_MOVES = 128U;

constexpr uChar MAX_DEPTH = 64U;

//files Bitmap
constexpr Bitmap FILES[8] = { 0x0101010101010101UL, 0x0202020202020202UL , 0x0404040404040404UL, 0x0808080808080808UL,
								0x1010101010101010UL, 0x2020202020202020UL, 0x4040404040404040UL, 0x8080808080808080UL };
//ranks Bitmap
constexpr Bitmap RANKS[8] = { 0x00000000000000FFUL, 0x000000000000FF00UL, 0x0000000000FF0000UL, 0x00000000FF000000UL,
								0x000000FF00000000UL, 0x0000FF0000000000UL, 0x00FF000000000000UL, 0xFF00000000000000UL };
//diagonal Bitmap from botton left to up right
constexpr Bitmap DIAGONAL_R[15] = { 0x0100000000000000UL, 0x0201000000000000UL, 0x0402010000000000UL, 0x0804020100000000UL,
									0x1008040201000000UL, 0x2010080402010000UL, 0x4020100804020100UL, 0x8040201008040201UL,
									0x0080402010080402UL, 0x0000804020100804UL, 0x0000008040201008UL, 0x0000000080402010UL,
									0x0000000000804020UL, 0x0000000000008040UL, 0x0000000000000080UL };
//diagonal Bitmap from botton right to up left
constexpr Bitmap DIAGONAL_L[15] = { 0x0000000000000001UL, 0x0000000000000102UL, 0x0000000000010204UL, 0x0000000001020408UL,
									0x0000000102040810UL, 0x0000010204081020UL, 0x0001020408102040UL, 0x0102040810204080UL,
									0x0204081020408000UL, 0x0408102040800000UL, 0x0810204080000000UL, 0x1020408000000000UL,
									0x2040800000000000UL, 0x4080000000000000UL, 0x8000000000000000UL };

constexpr int SQUARE_VALUE[6][64] = {
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

extern inline File& operator++(File& f) noexcept;
extern inline File& operator--(File& f) noexcept;
extern inline File operator~(const File& f) noexcept;

extern inline Rank& operator++(Rank& r) noexcept;
extern inline Rank& operator--(Rank& r) noexcept;
extern inline Rank operator~(const Rank& r) noexcept;

template <typename T>
	requires std::integral<T>
extern inline Square operator+(Square s, const T t) noexcept { return static_cast<Square>((static_cast<T>(s) + t) % 65U); }
template <typename T>
	requires std::integral<T>
extern inline Square operator-(Square s, const T t) noexcept { return static_cast<Square>((static_cast<T>(s) - (t % 64U)) % 64U); }
extern inline Square& operator++(Square& s) noexcept;
extern inline Square& operator--(Square& s) noexcept;
extern inline Square operator~(const Square& s) noexcept;

extern inline Piece& operator++(Piece& p) noexcept;
extern inline Piece& operator--(Piece& p) noexcept;

extern inline Color operator~(const Color& color) noexcept;

/* This function returns the File of Square*/
extern inline const File getFileOf(const Square square) noexcept;
/* This function returns the Rank of Square*/
extern inline const Rank getRankOf(const Square square) noexcept;
/* This function returns the Square of File and Rank*/
extern inline const Square getSquareOf(const File file, const Rank rank) noexcept;
/* This function returns a Bitmap of Square*/
extern inline const Bitmap getBitmapOf(const Square square) noexcept;
/*this function returns the position(Square) of the left most set bit*/
extern const Square getLastSquareOf(const Bitmap bitmap) noexcept;
/*this function returns and clear the position(Square) of the left most set bit*/
extern const Square popLastSquareOf(Bitmap& bitmap) noexcept;
/*this function returns the position(Square) of the right most set bit*/
extern const Square getFirstSquareOf(const Bitmap bitmap) noexcept;
/*this function returns and clear the position (Square) of the right most set bit*/
extern const Square popFirstSquareOf(Bitmap& bitmap) noexcept;

extern std::ostream& operator<<(std::ostream& os, const File& file);

extern std::ostream& operator<<(std::ostream& os, const Rank& rank);

extern std::ostream& operator<<(std::ostream& os, const Square& square);

/*This function does t1 & t2 != 0*/
template <typename T>
	requires std::integral<T>
extern inline const bool hasIntersection(const T t1, const T t2) noexcept {
	return (t1 & t2) != 0;
}
/*This function does t1 | t2*/
template <typename T>
	requires std::integral<T>
extern inline const T getUnion(const T t1, const T t2) noexcept {
	return t1 | t2;
}
/*This function does t1 & t2*/
template <typename T>
	requires std::integral<T>
extern inline const T getIntersections(const T t1, const T t2) noexcept {
	return t1 & t2;
}
/*This function does t1 ^ t2*/
template <typename T>
	requires std::integral<T>
extern inline const T getDiference(const T t1, const T t2) noexcept {
	return t1 ^ t2;
}
/*This function does t1 & ~t2*/
template <typename T>
	requires std::integral<T>
extern inline const T unsetIntersections(const T t1, const T t2) noexcept {
	return t1 & ~t2;
}

extern const uInt bitCount(Bitmap bitmap) noexcept;

extern const Bitmap reverse(Bitmap bitmap) noexcept;

template <typename T>
	requires std::integral<T>
extern void printBits(const char* title, const T t) {
	std::cout << title << std::endl;
	const uInt size = sizeof(T) * 8;

	for (uInt i = 0U, j = size - 1U; i < size; ++i, --j) {
		std::cout << (t >> j & 1);
		if (j % 8 == 0U)
			std::cout << ' ';
	}
	std::cout << std::endl;
}