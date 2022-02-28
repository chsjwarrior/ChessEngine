#pragma once
#include "BitBoard.h"

/* This namespace has functions that returns the attack bitmap for each piece */
namespace attacks {
	/* This function returns an attack bitmap for Bishop */
	const Bitmap getBishopMoves(const Bitmap allPieces, const File file, const Rank rank, const Bitmap squareBitmap);
	/* This function returns an attack bitmap for Rook */
	const Bitmap getRookMoves(const Bitmap allPieces, const File file, const Rank rank, const Bitmap squareBitmap);
	/* This function returns an attack bitmap for Knight */
	const Bitmap getKnightMoves(const Bitmap squareBitmap);
	/* This function returns an attack bitmap for King */
	const Bitmap getKingMoves(const Bitmap squareBitmap);
	/* This function returns an attack bitmap for Pawn */
	const Bitmap getPawnMoves(const Bitmap allPieces, const Color color, const Bitmap enPassantBitmap, const Bitmap squareBitmap);
	/* This function tests if the square is under attack by color */
	const bool isSquareAttacked(const BitBoard& bitBoard, const Color color, const Square square);
};