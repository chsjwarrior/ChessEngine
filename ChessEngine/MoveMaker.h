#pragma once
#include "Attacks.h"

/* This is a singleton class that make a move in bitBoard */
class MoveMaker {
	MoveMaker() = default;
	~MoveMaker() = default;
	/* This function remove the captured piece of the bitBoard */
	void checkCapture(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const;
	/* This function set or clear en passant square in bitBoard class */
	void checkEnPassant(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const;
	/* This function remove the captured pawn of the bitBoard */
	void checkEnPassantCaptured(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const;
	/* This function replace the pawn for the promotion piece */
	void checkPawnPromotion(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const;
	/* This function move the Rook to castle position */
	void checkCastleMove(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const;
	/* This function check the castle permission */
	void checkCastlePermission(BitBoard& bitBoard, const Square square, const Color color) const;
public:
	MoveMaker(const MoveMaker&) = delete;
	MoveMaker(MoveMaker&&) = delete;
	MoveMaker& operator=(const MoveMaker&) = delete;
	MoveMaker& operator=(MoveMaker&&) = delete;

	static MoveMaker& getInstance();
	/* This function undo the last move */
	void makeUndo(BitBoard& bitBoard) const;
	/* This function make a move */
	bool makeMove(BitBoard& bitBoard, const Move& move) const;
};