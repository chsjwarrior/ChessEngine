#pragma once
#include "Board.h"

namespace attacks {
	/* This function returns a bitmap with all squares reached by queen */
	BitBoard getQueenAttacks(const BitBoard allPieces, const File file, const Rank rank, const BitBoard square);
	/* This function returns a bitmap with all squares reached by bishop */
	BitBoard getBishopAttacks(const BitBoard allPieces, const File file, const Rank rank, const BitBoard square);
	/* This function returns a bitmap with all squares reached by rook */
	BitBoard getRookAttacks(const BitBoard allPieces, const File file, const Rank rank, const BitBoard square);
	/* This function returns a bitmap with all squares reached by knight */
	BitBoard getKnightAttacks(const BitBoard square);
	/* This function returns a bitmap with all squares reached by king */
	BitBoard getKingAttacks(const BitBoard square);
	/* This function returns a bitmap with the Pawn quiet moves mask */
	BitBoard getPawnMoves(const BitBoard allPieces, const Color color, const BitBoard square);
	/* This function returns a bitmap with the Pawn attacks mask */
	BitBoard getPawnAttacks(const Color color, const BitBoard square);
	/* This function returns a bitmap with the Pawn attacks mask */
	BitBoard getPawnAttacks(const BitBoard allPieces, const Color color, const BitBoard square);
	/* This function returns a bitmap with the Pawn En Passant attack mask */
	BitBoard getPawnEnPassantAttack(const Color color, const BitBoard enPassant, const BitBoard square);
	/* This function tests if the square is under attack by color */
	bool isSquareAttacked(const Board& board, const Color color, const Square square);
}