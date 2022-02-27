#pragma once
#include "BitBoard.h"

/* This namespace has functions that returns the attack bitmap for each piece */
namespace attacks {
	const Bitmap getBishopMoves(const Bitmap allPieces, const File file, const Rank rank, const Bitmap squareBitmap);

	const Bitmap getRookMoves(const Bitmap allPieces, const File file, const Rank rank, const Bitmap squareBitmap);

	const Bitmap getKnightMoves(const Bitmap squareBitmap);

	const Bitmap getKingMoves(const Bitmap squareBitmap);

	const Bitmap getPawnMoves(const Bitmap allPieces, const Color color, const Bitmap enPassantBitmap, const Bitmap squareBitmap);

	const bool isSquareAttacked(const BitBoard& bitBoard, const Color color, const Square square);
};