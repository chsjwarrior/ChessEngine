#pragma once
#include "Attacks.h"

namespace moveGenerator {
	/* This function generate moves by square, illegals included */
	uShort generateMoves(const BitBoard& bitBoard, Move moves[], const Square square);
	/* This function generate all moves, illegals included */
	uShort generateMoves(const BitBoard& bitBoard, Move moves[]);
	/* This function generate all capture moves, illegals included */
	uShort generateCaptureMoves(const BitBoard& bitBoard, Move moves[]);
	/* This function generate all legal moves */
	uShort generateLegalMoves(BitBoard& bitBoard, Move moves[]);
	/* This function verify if the move exists and legal */
	bool moveExists(BitBoard& bitBoard, const Move move);
};