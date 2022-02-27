#include "Perft.h"

void perft(BitBoard& bitBoard, const short depth, uLong& leafNodes) {
	if (depth <= 0) {
		++leafNodes;
		return;
	}

	MoveList list;
	MoveGenerator& moveGenerator = MoveGenerator::getInstance();
	moveGenerator.generateMoves(bitBoard, list);
	MoveMaker& moveMaker = MoveMaker::getInstance();

	for (Move* move = list.moves; move != list.moves + list.count; ++move) {
		if (!moveMaker.makeMove(bitBoard, *move))
			continue;

		perft(bitBoard, depth - 1, leafNodes);
		moveMaker.makeUndo(bitBoard);
	}
}

void perftTest(BitBoard& bitBoard, const short depth) {
	std::cout << bitBoard;
	std::cout << "Starting perft test to Depth: " << depth << std::endl;
	uLong leafNodes = 0UL;
	uLong cumnodes;

	MoveList list;
	MoveGenerator& moveGenerator = MoveGenerator::getInstance();
	moveGenerator.generateMoves(bitBoard, list);
	MoveMaker& moveMaker = MoveMaker::getInstance();

	for (Move* move = list.moves; move != list.moves + list.count; ++move) {
		if (!moveMaker.makeMove(bitBoard, *move))
			continue;

		cumnodes = leafNodes;
		perft(bitBoard, depth - 1, leafNodes);
		moveMaker.makeUndo(bitBoard);
		std::cout << *move << ": " << leafNodes - cumnodes << std::endl;
	}

	std::cout << "Test complete: " << std::endl;
	std::cout << leafNodes << " nodes visited" << std::endl;
}