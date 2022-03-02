#pragma once
#include "MoveMaker.h"

/* This is a singleton class that generate moves in bitBoard */
class MoveGenerator {
	Bitmap friendPieces;
	Bitmap enemyPieces;
	uShort movesCount;

	MoveGenerator() = default;
	~MoveGenerator() = default;

	/* This function sets the captured piece in the Move if any */
	void setCapture(const BitBoard& bitBoard, Move& move, const Color color, const Square to) const;
	/* This function tests if is possible to make King castle */
	bool canMakeKingCastle(const BitBoard& bitBoard, const Color color) const;
	/* This function tests if is possible to make Queen castle */
	bool canMakeQueenCastle(const BitBoard& bitBoard, const Color color) const;
	/* This function returns a bitmap with the attack squares */
	Bitmap getPiecesMoves(const BitBoard& bitBoard, const Piece piece, const Color color, const Square square) const;
	/* This function populates an array with the moves and returns the size */
	void catalogMoves(const BitBoard& bitBoard, Move moves[], const Piece piece, const Color color, const Square from, Bitmap attacks);

public:
	MoveGenerator(const MoveGenerator&) = delete;
	MoveGenerator(MoveGenerator&&) = delete;
	MoveGenerator& operator=(const MoveGenerator&) = delete;
	MoveGenerator& operator=(MoveGenerator&&) = delete;

	/* This function generate all moves, illegals included */
	uShort generateMoves(const BitBoard& bitBoard, Move moves[]);
	/* This function generate all capture moves, illegals included */
	uShort generateCaptureMoves(const BitBoard& bitBoard, Move moves[]);

	static MoveGenerator& getInstance();
};