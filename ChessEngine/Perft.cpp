#include "Chess.h"

static uLong perft(BitBoard& bitBoard, const short depth) {
	if (depth <= 0)
		return 1UL;

	if (info.stop)
		return 1UL;

	Move moves[MAX_MOVES];
	uShort movesCount = moveGenerator::generateAllLegalMoves(bitBoard, moves);
	uLong nodes = 0UL;

	for (Move* move = moves; move != moves + movesCount; ++move) {
		makeMove(bitBoard, *move);
		nodes += perft(bitBoard, depth - 1);
		makeUndo(bitBoard);

		if (info.stop == true)
			break;
	}

	return nodes;
}

void perftTest(BitBoard& bitBoard) {
	Move moves[MAX_MOVES];
	uShort movesCount = moveGenerator::generateAllLegalMoves(bitBoard, moves);
	uLong nodes;

	for (Move* move = moves; move != moves + movesCount; ++move) {
		makeMove(bitBoard, *move);

		nodes = perft(bitBoard, info.depth - 1);
		info.nodes += nodes;

		makeUndo(bitBoard);
		std::cout << *move << ": " << nodes << std::endl;

		if (info.stop)
			break;
	}

	std::cout << "Node searched: " << info.nodes << std::endl;
}