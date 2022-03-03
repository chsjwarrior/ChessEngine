#pragma once
#include "BitBoard.h"

/* This function returns an attack bitmap for Bishop */
Bitmap getBishopMoves(const Bitmap allPieces, const File file, const Rank rank, const Bitmap squareBitmap);
/* This function returns an attack bitmap for Rook */
Bitmap getRookMoves(const Bitmap allPieces, const File file, const Rank rank, const Bitmap squareBitmap);
/* This function returns an attack bitmap for Knight */
Bitmap getKnightMoves(const Bitmap squareBitmap);
/* This function returns an attack bitmap for King */
Bitmap getKingMoves(const Bitmap squareBitmap);
/* This function returns an attack bitmap for Pawn */
Bitmap getPawnMoves(const Bitmap allPieces, const Color color, const Bitmap enPassantBitmap, const Bitmap squareBitmap);
/* This function tests if the square is under attack by color */
bool isSquareAttacked(const BitBoard& bitBoard, const Color color, const Square square);