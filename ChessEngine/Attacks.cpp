#include "Attacks.h"

//files Bitmap
static const Bitmap FILES[8] = { 0x0101010101010101UL, 0x0202020202020202UL , 0x0404040404040404UL, 0x0808080808080808UL,
								0x1010101010101010UL, 0x2020202020202020UL, 0x4040404040404040UL, 0x8080808080808080UL };
//ranks Bitmap
static const Bitmap RANKS[8] = { 0x00000000000000FFUL, 0x000000000000FF00UL, 0x0000000000FF0000UL, 0x00000000FF000000UL,
								0x000000FF00000000UL, 0x0000FF0000000000UL, 0x00FF000000000000UL, 0xFF00000000000000UL };

static const Bitmap DIAGONAL[15] = { 0x0000000000000001UL, 0x0000000000000102UL, 0x0000000000010204UL, 0x0000000001020408UL,
									0x0000000102040810UL, 0x0000010204081020UL, 0x0001020408102040UL, 0x0102040810204080UL,
									0x0204081020408000UL, 0x0408102040800000UL, 0x0810204080000000UL, 0x1020408000000000UL,
									0x2040800000000000UL, 0x4080000000000000UL, 0x8000000000000000UL };
static const Bitmap ANTI_DIAGONAL[15] = { 0x0000000000000080UL, 0x0000000000008040UL, 0x0000000000804020UL, 0x0000000080402010UL,
									0x0000008040201008UL, 0x0000804020100804UL, 0x0080402010080402UL, 0x8040201008040201UL,
									0x4020100804020100UL, 0x2010080402010000UL, 0x1008040201000000UL, 0x0804020100000000UL,
									0x0402010000000000UL, 0x0201000000000000UL, 0x0100000000000000UL };

Bitmap attacks::getBishopMoves(const Bitmap allPieces, const File file, const Rank rank, const Bitmap squareBitmap) {
	const Bitmap reverseSquareBitmap = reverse(squareBitmap);

	uShort index = rank + file;

	Bitmap aux1 = (allPieces & DIAGONAL[index]) - 2 * squareBitmap;
	Bitmap aux2 = reverse(allPieces & DIAGONAL[index]) - 2 * reverseSquareBitmap;
	const Bitmap ldMoves = (aux1 ^ reverse(aux2)) & DIAGONAL[index];

	index = rank + 7 - file;

	aux1 = (allPieces & ANTI_DIAGONAL[index]) - 2 * squareBitmap;
	aux2 = reverse(allPieces & ANTI_DIAGONAL[index]) - 2 * reverseSquareBitmap;
	const Bitmap dlMoves = (aux1 ^ reverse(aux2)) & ANTI_DIAGONAL[index];

	return ldMoves | dlMoves;
}

Bitmap attacks::getRookMoves(const Bitmap allPieces, const File file, const Rank rank, const Bitmap squareBitmap) {
	const Bitmap reverseSquareBitmap = reverse(squareBitmap);

	Bitmap aux1 = allPieces - 2 * squareBitmap;
	Bitmap aux2 = reverse(allPieces) - 2 * reverseSquareBitmap;
	const Bitmap hMoves = (aux1 ^ reverse(aux2)) & RANKS[rank];

	aux1 = (allPieces & FILES[file]) - 2 * squareBitmap;
	aux2 = reverse(allPieces & FILES[file]) - 2 * reverseSquareBitmap;
	const Bitmap vMoves = (aux1 ^ reverse(aux2)) & FILES[file];

	return hMoves | vMoves;
}

Bitmap attacks::getKnightMoves(const Bitmap squareBitmap) {
	Bitmap moves = squareBitmap << 10 & ~(FILES[FILE_A] | FILES[FILE_B]);
	moves |= squareBitmap << 17 & ~FILES[FILE_A];
	moves |= squareBitmap << 6 & ~(FILES[FILE_G] | FILES[FILE_H]);
	moves |= squareBitmap << 15 & ~FILES[FILE_H];

	moves |= squareBitmap >> 10 & ~(FILES[FILE_G] | FILES[FILE_H]);
	moves |= squareBitmap >> 17 & ~FILES[FILE_H];
	moves |= squareBitmap >> 6 & ~(FILES[FILE_A] | FILES[FILE_B]);
	moves |= squareBitmap >> 15 & ~FILES[FILE_A];

	return moves;
}

Bitmap attacks::getKingMoves(const Bitmap squareBitmap) {
	Bitmap moves = squareBitmap << 8 | squareBitmap >> 8;
	moves |= squareBitmap << 1 & ~FILES[FILE_A];
	moves |= squareBitmap << 9 & ~FILES[FILE_A];
	moves |= squareBitmap << 7 & ~FILES[FILE_H];

	moves |= squareBitmap >> 1 & ~FILES[FILE_H];
	moves |= squareBitmap >> 9 & ~FILES[FILE_H];
	moves |= squareBitmap >> 7 & ~FILES[FILE_A];

	return moves;
}

Bitmap attacks::getPawnMoves(const Bitmap allPieces, const Color color, const Bitmap enPassantBitmap, const Bitmap squareBitmap) {
	Bitmap moves = 0UL;
	if (color == WHITE) {
		//normal move
		moves |= squareBitmap << 8 & ~allPieces;
		//start move
		moves |= squareBitmap << 16 & ~allPieces & ~allPieces << 8 & RANKS[RANK_4];
		//capture move
		moves |= squareBitmap << 7 & allPieces & ~FILES[FILE_H];
		moves |= squareBitmap << 9 & allPieces & ~FILES[FILE_A];
		//en passant capture move
		moves |= squareBitmap << 7 & enPassantBitmap & RANKS[RANK_6] & ~FILES[FILE_H];
		moves |= squareBitmap << 9 & enPassantBitmap & RANKS[RANK_6] & ~FILES[FILE_A];
	} else {
		//normal move
		moves |= squareBitmap >> 8 & ~allPieces;
		//start move
		moves |= squareBitmap >> 16 & ~allPieces & ~allPieces >> 8 & RANKS[RANK_5];
		//capture move
		moves |= squareBitmap >> 7 & allPieces & ~FILES[FILE_A];
		moves |= squareBitmap >> 9 & allPieces & ~FILES[FILE_H];
		//en passant capture move
		moves |= squareBitmap >> 7 & enPassantBitmap & RANKS[RANK_3] & ~FILES[FILE_A];
		moves |= squareBitmap >> 9 & enPassantBitmap & RANKS[RANK_3] & ~FILES[FILE_H];
	}
	return moves;
}

bool attacks::isSquareAttacked(const BitBoard& bitBoard, const Color color, const Square square) {
	const Bitmap squareBitmap = getBitmapOf(square);
	//King
	Bitmap attacks = getKingMoves(squareBitmap);
	if (attacks & bitBoard.getBitmapPiece(KING, color))
		return true;
	//Knight
	attacks = getKnightMoves(squareBitmap);
	if (attacks & bitBoard.getBitmapPiece(KNIGHT, color))
		return true;
	//Pawn
	if (color == WHITE)
		attacks = squareBitmap >> 7 & ~FILES[FILE_A] | squareBitmap >> 9 & ~FILES[FILE_H];
	else
		attacks = squareBitmap << 7 & ~FILES[FILE_H] | squareBitmap << 9 & ~FILES[FILE_A];
	if (attacks & bitBoard.getBitmapPiece(PAWN, color))
		return true;

	const File file = getFileOf(square);
	const Rank rank = getRankOf(square);
	const Bitmap allPieces = bitBoard.getBitmapAllPieces(WHITE) | bitBoard.getBitmapAllPieces(BLACK);

	//Bishop
	attacks = getBishopMoves(allPieces, file, rank, squareBitmap);
	if (attacks & bitBoard.getBitmapPiece(BISHOP, color) ||
		attacks & bitBoard.getBitmapPiece(QUEEN, color))
		return true;
	//Rook
	attacks = getRookMoves(allPieces, file, rank, squareBitmap);
	if (attacks & bitBoard.getBitmapPiece(ROOK, color) ||
		attacks & bitBoard.getBitmapPiece(QUEEN, color))
		return true;
	return false;
}