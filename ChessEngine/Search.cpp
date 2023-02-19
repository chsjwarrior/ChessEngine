#include "Chess.h"
#include "Evaluator.h"

static void checkUp() {
	auto now = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
	info.stop = info.stopTime > 0 && info.startTime + duration.count() > info.stopTime;
}

static void swapForBestMove(const uShort index, Move moves[], const uShort size) {
	int bestScore = -INFINIT;
	int bestIndex = 0;

	for (uShort i = index; i < size; ++i)
		if (moves[i].score > bestScore) {
			bestScore = moves[i].score;
			bestIndex = i;
		}

	std::swap(moves[index], moves[bestIndex]);
}

static int quiesce(BitBoard& bitBoard, int alpha, int beta) {
	if (info.nodes < 2047)
		checkUp();

	++info.nodes;

	if (bitBoard.isRepetition() || bitBoard.getFiftyMove() >= 100)
		return 0;

	int score = evaluatePosition(bitBoard);

	if (bitBoard.getPly() > MAX_DEPTH - 1)
		return score;

	if (score >= beta)
		return beta;

	if (score > alpha)
		alpha = score;

	Move moves[MAX_MOVES];
	uShort moveCount = moveGenerator::generateCaptureMoves(bitBoard, moves);

	for (uShort i = 0; i < moveCount; ++i) {
		swapForBestMove(i, moves, moveCount);

		if (!makeMove(bitBoard, moves[i]))
			continue;

		score = -quiesce(bitBoard, -beta, -alpha);
		makeUndo(bitBoard);

		if (info.stop == true)
			return 0;

		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

static int alphaBeta(BitBoard& bitBoard, short depth, int alpha, int beta) {
	if (depth <= 0)
		return quiesce(bitBoard, alpha, beta);

	info.nodes++;

	if (info.nodes >= 2047U)
		checkUp();

	if (bitBoard.isRepetition() || bitBoard.getFiftyMove() >= 100 && bitBoard.getPly())
		return 0;

	if (bitBoard.getPly() > MAX_DEPTH - 1)
		return evaluatePosition(bitBoard);

	{
		TranspositionTableEntry* t = retrieveTranspositionTableEntry(bitBoard.getHashkey());
		if (t != nullptr) {
			switch (t->nodeType) {
			case HFALPHA:
				return alpha;
			case HFBETA:
				return beta;
			case HFEXACT:
				if (t->score > INFINIT + MAX_DEPTH)
					return t->score - bitBoard.getPly();
				else if (t->score < -(INFINIT + MAX_DEPTH))
					return t->score + bitBoard.getPly();
				return t->score;
			}
		}
	}

	const Color color = bitBoard.getColorTime();
	const bool inCheck = attacks::isSquareAttacked(bitBoard, ~color, getFirstSquareOf(bitBoard.getBitmapPiece(KING, color)));

	if (inCheck)
		++depth;

	uInt legal = 0U;
	int score, oldAlpha = alpha, bestScore = -INFINIT;
	Move bestMove;

	Move moves[MAX_MOVES];
	uShort moveCount = moveGenerator::generateMoves(bitBoard, moves);

	for (uShort i = 0; i < moveCount; ++i) {
		swapForBestMove(i, moves, moveCount);

		if (!makeMove(bitBoard, moves[i]))
			continue;

		++legal;
		score = -alphaBeta(bitBoard, depth - 1, -beta, -alpha);
		makeUndo(bitBoard);

		if (info.stop == true)
			return 0;

		if (score > bestScore) {
			bestScore = score;
			bestMove = moves[i];
			if (score > alpha) {
				if (score >= beta) {
					storeTranspositionTableEntry(bitBoard.getHashkey(), depth, beta, HFBETA, moves[i]);
					return beta;
				}
				alpha = score;
			}
		}
	}

	if (legal == 0) {
		if (inCheck)
			return -INFINIT + bitBoard.getPly();
		return 0;
	}
	if (alpha != oldAlpha)
		storeTranspositionTableEntry(bitBoard.getHashkey(), depth, bestScore, HFEXACT, bestMove);
	else
		storeTranspositionTableEntry(bitBoard.getHashkey(), depth, alpha, HFALPHA, bestMove);
	return alpha;
}

static uShort generatePvLine(BitBoard& bitBoard, Move moves[], short depth) {
	TranspositionTableEntry* t = retrieveTranspositionTableEntry(bitBoard.getPly());
	int count = 0;
	Move move;

	if (t != nullptr)
		move = t->move;

	while (move.isEmpty() == false && count < depth) {
		if (moveGenerator::moveExists(bitBoard, move)) {
			makeMove(bitBoard, move);
			moves[count++] = move;
		} else
			break;

		t = retrieveTranspositionTableEntry(bitBoard.getHashkey());
		if (t == nullptr)
			move();
		else
			move = t->move;
	}
	while (bitBoard.getPly() > 0)
		makeUndo(bitBoard);

	return count;
}

void searchPosition(BitBoard& bitBoard) {
	bitBoard.ply = 0U;
	Move pvLine[MAX_DEPTH];
	uShort pvSize;
	int score;

	for (short d = 1; d <= info.depth; ++d) {
		score = alphaBeta(bitBoard, d, -INFINIT, INFINIT);

		if (info.stop)
			d = info.depth;

		pvSize = generatePvLine(bitBoard, pvLine, d);

		std::cout << "info score " << score << " depth " << d << " nodes " << info.nodes << std::endl;
		std::cout << "pvLies: ";
		for (uShort i = 0U; i < pvSize; ++i)
			std::cout << pvLine[i] << " ";
		std::cout << "\nTransposition table: ";
		for (TranspositionTableEntry& t : transpositionTable)
			if (t.hash != 0)
				std::cout << t.move << std::endl;
		std::cout << "\n";
	}
	std::cout << "bestmove: " << pvLine[0] << std::endl;
}