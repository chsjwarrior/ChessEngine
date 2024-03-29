#pragma once
#include "Attacks.h"

namespace moveGenerator {
	/* This function generates all moves type from the square */
	uShort generateAllMoves(const Board& board, Move moves[], const Square square);
	/* This function generates all moves type, illegals included */
	uShort generateAllMoves(const Board& board, Move moves[]);
	/* This function generates only quite moves, illegals include */
	uShort generateQuietMoves(const Board& board, Move moves[]);
	/* This function generates only capture moves, illegals included */
	uShort generateCaptureMoves(const Board& board, Move moves[]);
	/* This function verifies if the move exists and is legal */
	bool moveExists(Board& board, const Move move);
};