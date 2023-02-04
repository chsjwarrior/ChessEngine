#pragma once
#include "Attacks.h"

/* This is a singleton class that make a move in bitBoard */
class MoveMaker {
	MoveMaker() = default;
	~MoveMaker() = default;
	/* This function set or clear en passant square in bitBoard class */
	void checkEnPassant(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const;
public:
	MoveMaker(MoveMaker&) = delete;
	MoveMaker(MoveMaker&&) = delete;
	MoveMaker& operator=(MoveMaker&) = delete;
	MoveMaker& operator=(MoveMaker&&) = delete;

	static MoveMaker& getInstance();
	
};