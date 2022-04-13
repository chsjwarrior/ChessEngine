#pragma once
#include <chrono>
#include "MoveGenerator.h"

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

void searchPosition(BitBoard& bitBoard);

void perftTest(BitBoard& bitBoard);