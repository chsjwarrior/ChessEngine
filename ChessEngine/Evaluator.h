#pragma once
#include "BitBoard.h"

void sortMoves(const BitBoard& bitBoard, Move moves[], const uShort movesCount);

int evaluatePosition(const BitBoard& bitBoard);