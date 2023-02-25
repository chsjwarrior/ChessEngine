#pragma once
#include "BitBoard.h"

namespace attacks {
	/* This function returns a bitmap with the Bishop attacks mask */
	Bitmap getBishopAttacks(const Bitmap allPieces, const File file, const Rank rank, const Bitmap squareBitmap);
	/* This function returns a bitmap with the Rook attacks mask */
	Bitmap getRookAttacks(const Bitmap allPieces, const File file, const Rank rank, const Bitmap squareBitmap);
	/* This function returns a bitmap with the Knight attacks mask */
	Bitmap getKnightAttacks(const Bitmap squareBitmap);
	/* This function returns a bitmap with the King attacks mask */
	Bitmap getKingAttacks(const Bitmap squareBitmap);
	/* This function returns a bitmap with the Pawn attacks mask */
	Bitmap getPawnAttacks(const Bitmap enemyPieces, const Color color, const Bitmap enPassantBitmap, const Bitmap squareBitmap);
	/* This function returns a bitmap with the Pawn quiet moves mask */
	Bitmap getPawnMoves(const Bitmap allPieces, const Color color, const Bitmap squareBitmap);
	/* This function tests if the square is under attack by color */
	bool isSquareAttacked(const BitBoard& bitBoard, const Color color, const Square square);
}