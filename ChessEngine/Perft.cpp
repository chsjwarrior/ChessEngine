#include "Chess.h"

static uLong perft(Board& board, const short depth) {
	if (depth <= 0)
		return 1ULL;

	if (info.stop)
		return 1ULL;

	Move moves[MAX_MOVES];
	moveGenerator::generateAllMoves(board, moves);
	uLong nodes = 0ULL;

	for (Move& move : moves) {
		if (!makeMove(board, move))
			continue;

		nodes += perft(board, depth - 1);
		makeUndo(board);

		if (info.stop)
			break;
	}

	return nodes;
}

void perftTest(Board& board) {
	Move moves[MAX_MOVES];
	moveGenerator::generateAllMoves(board, moves);
	uLong nodes = 0ULL;

	for (Move& move : moves) {
		if (!makeMove(board, move))
			continue;

		nodes = perft(board, info.depth - 1);
		info.nodes += nodes;

		makeUndo(board);
		std::cout << move << ": " << nodes << std::endl;

		if (info.stop)
			break;
	}

	std::cout << "Node searched: " << info.nodes << std::endl;
}