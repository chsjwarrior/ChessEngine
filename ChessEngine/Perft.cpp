#include "Chess.h"

static uLong perft(BitBoard& bitBoard, const short depth) {
	if (depth <= 0)
		return 1UL;

	Move moves[MAX_MOVES];
	uShort movesCount = moveGenerator::generateMoves(bitBoard, moves);
	uLong nodes = 0UL;
	MoveMaker& moveMaker = MoveMaker::getInstance();

	for (Move* move = moves; move != moves + movesCount; ++move) {
		if (!moveMaker.makeMove(bitBoard, *move))
			continue;

		nodes += perft(bitBoard, depth - 1);
		moveMaker.makeUndo(bitBoard);
	}
	return nodes;
}

void perftTest(BitBoard& bitBoard) {
	Move moves[MAX_MOVES];
	uShort movesCount = moveGenerator::generateMoves(bitBoard, moves);
	uLong nodes;
	MoveMaker& moveMaker = MoveMaker::getInstance();

	for (Move* move = moves; move != moves + movesCount; ++move) {
		if (!moveMaker.makeMove(bitBoard, *move))
			continue;

		nodes = perft(bitBoard, info.depth - 1);
		info.nodes += nodes;
		moveMaker.makeUndo(bitBoard);
		std::cout << *move << ": " << nodes << std::endl;
	}

	std::cout << "Node searched: " << info.nodes << std::endl;
}