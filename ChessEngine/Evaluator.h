#pragma once
#include "Board.h"

void sortMoves(const Board& board, Move moves[], const uShort movesCount);

int evaluatePosition(const Board& board);