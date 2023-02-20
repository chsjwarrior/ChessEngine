#include "chess.h"

static int getIndex(uLong hash) {
	return hash % 0x100000;
}

void clearTranspositionTable() {
	for (TranspositionTableEntry& t : transpositionTable) {
		t.hash = 0;
		t.depth = -1;
		t.score = 0;
		t.nodeType = HFNONE;
		t.move();
	}
}

void storeTranspositionTableEntry(uLong hash, short depth, int score, NodeType nodeType, Move bestMove) {
	int index = getIndex(hash);
	transpositionTable[index].hash = hash;
	transpositionTable[index].depth = depth;
	transpositionTable[index].score = score;
	transpositionTable[index].nodeType = nodeType;
	transpositionTable[index].move = bestMove;
}

TranspositionTableEntry* retrieveTranspositionTableEntry(uLong hash) {
	int index = getIndex(hash);
	if (transpositionTable[index].hash == hash)
		return &transpositionTable[index];
	return nullptr;
}