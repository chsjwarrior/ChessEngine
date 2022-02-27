#pragma once
#include "Types.h"



struct SearchInfo {
	int startTime = 0;
	int stopTime = 0;
	int moveTime = 0;
	int time = 0;
	int inc = 0;
	int movestogo = 0;
	short depth = 1;
	int mate = 0;
	uLong nodes = 0UL;
	uLong infinite = 0UL;

	bool ponderMode = false;
	bool stop = false;
};