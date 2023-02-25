#include "Chess.h"
#include "Evaluator.h"

static void checkUp() {
	auto now = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
	info.stop = info.stopTime > 0 && info.startTime + duration.count() > info.stopTime;
}

static void clearSearch() {
	clearTranspositionTable();
	info.fh = 0;
	info.fhf = 0;
}

static void swapForBestMove(const uShort index, Move moves[], const uShort size) {
	int bestScore = -INFINIT;
	uShort bestIndex = 0;

	for (uShort i = index; i < size; ++i)		
		if (moves[i].score > bestScore) {
			bestScore = moves[i].score;
			bestIndex = i;
		}

	std::swap(moves[index], moves[bestIndex]);
}

static int quiesce(BitBoard& bitBoard, int alpha, int beta) {
	if (info.nodes >= 2047)
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
	uInt legal = 0U;

	for (uShort i = 0; i < moveCount; ++i) {
		swapForBestMove(i, moves, moveCount);

		if (!makeMove(bitBoard, moves[i]))
			continue;

		++legal;
		score = -quiesce(bitBoard, -beta, -alpha);
		makeUndo(bitBoard);

		if (info.stop)
			return 0;

		if (score > alpha) {
			if (score >= beta) {
				if (legal == 1)
					++info.fhf;
				++info.fh;
				return beta;
			}
			alpha = score;
		}
	}
	return alpha;
}

static int alphaBeta(BitBoard& bitBoard, short depth, int alpha, int beta) {
	if (depth <= 0)
		return evaluatePosition(bitBoard);
	//return quiesce(bitBoard, alpha, beta);

	if (info.nodes >= 2047U)
		checkUp();

	++info.nodes;

	if (bitBoard.isRepetition() || bitBoard.getFiftyMove() >= 100 && bitBoard.getPly())
		return 0;

	if (bitBoard.getPly() > MAX_DEPTH - 1)
		return evaluatePosition(bitBoard);
	/*
	{
		TranspositionTableEntry* t = retrieveTranspositionTableEntry(bitBoard.getHashkey());
		if (t != nullptr) {
			switch (t->nodeType) {
			case HFALPHA:
				return alpha;
			case HFBETA:
				return beta;
			case HFEXACT:
				if (t->score > MATE)
					return t->score - bitBoard.getPly();
				else if (t->score < -MATE)
					return t->score + bitBoard.getPly();
				return t->score;
			}
		}
	}
	*/
	const Color color = bitBoard.getColorTime();
	const bool inCheck = attacks::isSquareAttacked(bitBoard, ~color, getFirstSquareOf(bitBoard.getBitmapPiece(KING, color)));

	if (inCheck)
		++depth;

	Move moves[MAX_MOVES];
	uShort moveCount = moveGenerator::generateAllMoves(bitBoard, moves);
	uInt legal = 0U;
	int score, oldAlpha = alpha, bestScore = -INFINIT;
	Move bestMove;

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
					if (legal == 1)
						++info.fhf;
					++info.fh;
					storeTranspositionTableEntry(bitBoard.getHashkey(), depth, beta, HFBETA, bestMove);
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
	TranspositionTableEntry* t = retrieveTranspositionTableEntry(bitBoard.getHashkey());
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
	clearSearch();
	bitBoard.ply = 0U;

	Move bestMove;
	Move pvLine[MAX_DEPTH];
	uShort pvSize;
	int score;

	for (short d = 1; d <= info.depth; ++d) {
		score = alphaBeta(bitBoard, d, -INFINIT, INFINIT);

		if (info.stop)
			d = info.depth;

		pvSize = generatePvLine(bitBoard, pvLine, d);
		bestMove = pvLine[0];

		std::cout << "info score " << score << " depth " << d << " nodes " << info.nodes << std::endl;
		std::cout << "pvLine: ";
		for (uShort i = 0U; i < pvSize; ++i)
			std::cout << pvLine[i] << " ";

		std::cout << std::endl;
	}
	std::cout << "bestmove: " << pvLine[0] << std::endl;
}