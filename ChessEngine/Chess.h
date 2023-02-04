#pragma once
#include <chrono>
#include "MoveGenerator.h"
#include "MoveMaker.h"

struct SearchInfo {
	long long startTime;
	long long stopTime;
	long long moveTime;
	long long time;
	int inc;
	int movestogo;
	short depth;
	short perft;
	int mate;
	uLong nodes;
	uLong infinite;
	bool ponderMode;
	bool stop;
};

inline SearchInfo info;

/* This function undo the last move */
void makeUndo(BitBoard& bitBoard);
/* This function make a move */
bool makeMove(BitBoard& bitBoard, const Move& move);

void searchPosition(BitBoard& bitBoard);

void perftTest(BitBoard& bitBoard);