#include "Chess.h"

void checkUp() {
	//if (info.stopTime > 0 && getTimeMilliseconds() > info.stopTime)
		//info.stop = true;
}

void swapForBestMove(const uShort index, Move moves[], const uShort size) {
	int bestScore = -30000;
	int bestIndex = 0;

	for (uShort i = index; i < size; ++i)
		if (moves[i].score > bestScore) {
			bestScore = moves[i].score;
			bestIndex = i;
		}

	std::swap(moves[index], moves[bestIndex]);
}

Move bestMove;

const int quiescence(BitBoard& bitBoard, int alpha, int beta) {
	if (info.nodes >= 2047)
		checkUp();

	info.nodes++;

	if (bitBoard.isRepetition() || bitBoard.getFiftyMove() >= 100)
		return 0;

	if (bitBoard.getPly() > MAX_DEPTH - 1)
		return 0; //evaluatePosition(bitBoard);

	int score = 0;//evaluate::evaluatePosition(bitBoard);

	if (score >= beta)
		return beta;

	if (score > alpha)
		alpha = score;

	Move moves[MAX_MOVES];
	MoveGenerator& moveGenerator = MoveGenerator::getInstance();
	uShort movesCount = moveGenerator.generateCaptureMoves(bitBoard, moves);
	MoveMaker& moveMaker = MoveMaker::getInstance();

	for (uShort i = 0; i < movesCount; ++i) {
		swapForBestMove(i, moves, movesCount);

		if (!moveMaker.makeMove(bitBoard, moves[i]))
			continue;

		score = -quiescence(bitBoard, -beta, -alpha);
		moveMaker.makeUndo(bitBoard);

		if (info.stop)
			return 0;

		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

const int negaMax(BitBoard& bitBoard, short depth, int alpha, int beta) {
	info.nodes++;

	if (depth <= 0)
		return quiescence(bitBoard, alpha, beta);

	if (info.nodes >= 2047)
		checkUp();

	if (bitBoard.isRepetition() || bitBoard.getFiftyMove() >= 100 && bitBoard.getPly())
		return 0;

	if (bitBoard.getPly() > MAX_DEPTH - 1)
		return 0;// evaluate::evaluatePosition(bitBoard);

	const Color color = bitBoard.getColorTime();
	const bool inCheck = attacks::isSquareAttacked(bitBoard, ~color, getFirstSquareOf(bitBoard.getBitmapPiece(KING, color)));

	if (inCheck)
		++depth;

	uInt legal = 0U;
	int score = -30000;
	int bestScore = score;
	//Move bestMove;

	Move moves[MAX_MOVES];
	MoveGenerator& moveGenerator = MoveGenerator::getInstance();
	uShort moveCount = moveGenerator.generateMoves(bitBoard, moves);
	MoveMaker& moveMaker = MoveMaker::getInstance();

	for (uShort i = 0; i < moveCount; ++i) {
		swapForBestMove(i, moves, moveCount);

		if (!moveMaker.makeMove(bitBoard, moves[i]))
			continue;

		legal++;
		score = -negaMax(bitBoard, depth - 1, -beta, -alpha);
		moveMaker.makeUndo(bitBoard);

		if (info.stop == true)
			return 0;

		if (score > bestScore) {
			bestScore = score;
			bestMove = moves[i];
			if (score > alpha) {
				alpha = score;
				if (score >= beta) {
					return beta;
				}
			}
		}
	}

	if (legal == 0) {
		if (inCheck)
			return -29000 + bitBoard.getPly();
		return 0;
	}
	return alpha;
}

void searchPosition(BitBoard& bitBoard) {
	info.nodes = 0UL;
	info.stop = false;

	int score = -30000;

	for (short d = 1; d <= info.depth; ++d) {
		score = negaMax(bitBoard, d - 1, -30000, 30000);
		if (info.stop)
			break;

		std::cout << "info score " << score << " depth " << d << " nodes " << info.nodes;
		//std::cout << " time " << getTimeMilliseconds() - info.startTime << std::endl;
	}

	std::cout << "bestmove: " << bestMove << std::endl;
}