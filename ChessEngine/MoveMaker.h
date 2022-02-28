#pragma once
#include "Attacks.h"

/* This is a singleton class that make a move in bitBoard */
class MoveMaker {
	MoveMaker() = default;
	~MoveMaker() = default;

	void checkCapture(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const;

	void checkEnPassant(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const;

	void checkEnPassantCaptured(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const;

	void checkPawnPromotion(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const;

	void checkCastleMove(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const;

public:
	MoveMaker(const MoveMaker&) = delete;
	MoveMaker(MoveMaker&&) = delete;
	MoveMaker& operator=(const MoveMaker&) = delete;
	MoveMaker& operator=(MoveMaker&&) = delete;

	static MoveMaker& getInstance();

	void makeUndo(BitBoard& bitBoard) const;

	const bool makeMove(BitBoard& bitBoard, const Move& move) const;
};