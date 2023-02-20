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

	float fh;//fail high;
	float fhf;//fail high first;
};

enum NodeType { HFNONE, HFALPHA, HFBETA, HFEXACT };

struct TranspositionTableEntry {
	uLong hash;
	short depth;
	int score;
	NodeType nodeType;
	Move move;
};

inline SearchInfo info;

inline Move searchHistory[NONE_PIECE][2][NONE_PIECE];
inline Move searchKiller[2][NONE_PIECE];

inline TranspositionTableEntry transpositionTable[0x100000];

void clearTranspositionTable();

void storeTranspositionTableEntry(uLong hash, short depth, int score, NodeType nodeType, Move bestMove);

TranspositionTableEntry* retrieveTranspositionTableEntry(uLong hash);
/* This function undo the last move */
void makeUndo(BitBoard& bitBoard);
/* This function make a move */
bool makeMove(BitBoard& bitBoard, const Move& move);
/* This function make the search */
void searchPosition(BitBoard& bitBoard);
/*This function make a perft test */
void perftTest(BitBoard& bitBoard);