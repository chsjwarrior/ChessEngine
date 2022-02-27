#pragma once
#include "Attacks.h"

class MoveGenerator {
	Bitmap friendPieces;
	Bitmap enemyPieces;

	MoveGenerator() = default;
	~MoveGenerator() = default;

	void setCapture(const BitBoard& bitBoard, Move& move, const Color color, const Square to) const;

	const bool canMakeKingCastle(const BitBoard& bitBoard, const Color color) const;

	const bool canMakeQueenCastle(const BitBoard& bitBoard, const Color color) const;

	const Bitmap generatePiecesMoves(const BitBoard& bitBoard, const Piece piece, const Color color, const Square square) const;

	void catalogMoves(const BitBoard& bitBoard, MoveList& list, const Piece piece, const Color color, const Square from, Bitmap attacks) const;

public:
	MoveGenerator(MoveGenerator&) = delete;
	MoveGenerator(MoveGenerator&&) = delete;
	MoveGenerator& operator=(MoveGenerator&) = delete;
	MoveGenerator& operator=(MoveGenerator&&) = delete;

	void generateMoves(const BitBoard& bitBoard, MoveList& list);

	static MoveGenerator& getInstance();
};