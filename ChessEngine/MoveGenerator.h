#pragma once
#include "Attacks.h"

namespace moveGenerator {
	/* This function generate all moves type by square, illegals included */
	uShort generateAllMoves(const BitBoard& bitBoard, Move moves[], const Square square);
	/* This function generate all moves type , illegals included */
	uShort generateAllMoves(const BitBoard& bitBoard, Move moves[]);
	/* This function generate only quite moves, illegals include */
	uShort generateQuiteMoves(const BitBoard& bitBoard, Move moves[]);
	/* This function generate only capture moves, illegals included */
	uShort generateCaptureMoves(const BitBoard& bitBoard, Move moves[]);
	/* This function generate all legal moves */
	uShort generateAllLegalMoves(BitBoard& bitBoard, Move moves[]);
	/* This function verify if the move exists and is legal */
	bool moveExists(BitBoard& bitBoard, const Move move);
};