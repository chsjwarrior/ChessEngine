#pragma once
#include "MoveMaker.h"

/* This is a singleton class that generate moves in bitBoard */
class MoveGenerator {
	uShort moveCount;
	Bitmap friendPieces;
	Bitmap enemyPieces;

	MoveGenerator() = default;
	~MoveGenerator() = default;

	void setCapture(const BitBoard& bitBoard, Move& move, const Color color, const Square to) const;

	const bool canMakeKingCastle(const BitBoard& bitBoard, const Color color) const;

	const bool canMakeQueenCastle(const BitBoard& bitBoard, const Color color) const;

	const Bitmap generatePiecesMoves(const BitBoard& bitBoard, const Piece piece, const Color color, const Square square) const;

	void catalogMoves(const BitBoard& bitBoard, Move moves[], const Piece piece, const Color color, const Square from, Bitmap attacks);

public:
	MoveGenerator(const MoveGenerator&) = delete;
	MoveGenerator(MoveGenerator&&) = delete;
	MoveGenerator& operator=(const MoveGenerator&) = delete;
	MoveGenerator& operator=(MoveGenerator&&) = delete;

	const uShort generateMoves(const BitBoard& bitBoard, Move moves[]);

	const uShort generateCaptureMoves(const BitBoard& bitBoard, Move moves[]);

	static MoveGenerator& getInstance();
};