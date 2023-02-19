#pragma once
#include "BitBoard.h"

namespace attacks {
	/* This function returns a bitmap with Bishop moves */
	Bitmap getBishopMoves(const Bitmap allPieces, const File file, const Rank rank, const Bitmap squareBitmap);
	/* This function returns a bitmap with Rook */
	Bitmap getRookMoves(const Bitmap allPieces, const File file, const Rank rank, const Bitmap squareBitmap);
	/* This function returns a bitmap with Knight */
	Bitmap getKnightMoves(const Bitmap squareBitmap);
	/* This function returns a bitmap with King */
	Bitmap getKingMoves(const Bitmap squareBitmap);
	/* This function returns a bitmap with Pawn */
	Bitmap getPawnMoves(const Bitmap allPieces, const Color color, const Bitmap enPassantBitmap, const Bitmap squareBitmap);
	/* This function tests if the square is under attack by color */
	bool isSquareAttacked(const BitBoard& bitBoard, const Color color, const Square square);
}