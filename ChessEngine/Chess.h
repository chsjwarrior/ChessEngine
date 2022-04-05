#pragma once
#include <chrono>
#include "MoveGenerator.h"

struct SearchInfo {
	long long startTime = 0L;
	long long stopTime = 0L;
	long long moveTime = 0L;
	long long time = 0L;
	int inc = 0;
	int movestogo = 0;
	short depth = 1;
	short perft = 0;
	int mate = 0;
	uLong nodes = 0UL;
	uLong infinite = 0UL;

	bool ponderMode = false;
	bool stop = false;
};

inline SearchInfo info;

void searchPosition(BitBoard& bitBoard);

void perftTest(BitBoard& bitBoard);