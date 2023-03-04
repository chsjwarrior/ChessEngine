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

static int quiesce(Board& board, int alpha, int beta) {
	if (info.nodes >= 2047)
		checkUp();

	++info.nodes;

	if (board.isRepetition() || board.getFiftyMove() >= 100)
		return 0;

	int score = evaluatePosition(board);

	if (board.getPly() > MAX_DEPTH - 1)
		return score;

	if (score >= beta)
		return beta;

	if (score > alpha)
		alpha = score;

	Move moves[MAX_MOVES];
	uShort movesCount = moveGenerator::generateCaptureMoves(board, moves);
	sortMoves(board, moves, movesCount);
	uInt legal = 0U;

	for (uShort i = 0; i < movesCount; ++i) {
		if (!makeMove(board, moves[i]))
			continue;

		++legal;
		score = -quiesce(board, -beta, -alpha);
		makeUndo(board);

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

static int alphaBeta(Board& board, short depth, int alpha, int beta) {
	if (depth <= 0)
		return evaluatePosition(board);
	//return quiesce(bitBoard, alpha, beta);

	if (info.nodes >= 2047U)
		checkUp();

	++info.nodes;

	if (board.isRepetition() || board.getFiftyMove() >= 100 && board.getPly())
		return 0;

	if (board.getPly() > MAX_DEPTH - 1)
		return evaluatePosition(board);
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
	const Color color = board.getColorTime();
	const bool inCheck = attacks::isSquareAttacked(board, ~color, getFirstSquareOf(board.getBitBoardOfPiece(KING, color)));

	if (inCheck)
		++depth;

	Move moves[MAX_MOVES];
	uShort movesCount = moveGenerator::generateAllMoves(board, moves);
	sortMoves(board, moves, movesCount);
	uInt legal = 0U;
	int score, oldAlpha = alpha, bestScore = -INFINIT;
	Move bestMove;

	for (uShort i = 0; i < movesCount; ++i) {
		if (!makeMove(board, moves[i]))
			continue;

		++legal;
		score = -alphaBeta(board, depth - 1, -beta, -alpha);
		makeUndo(board);

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
					storeTranspositionTableEntry(board.getHashkey(), depth, beta, HFBETA, bestMove);
					return beta;
				}
				alpha = score;
			}
		}
	}

	if (legal == 0) {
		if (inCheck)
			return -INFINIT + board.getPly();
		return 0;
	}
	if (alpha != oldAlpha)
		storeTranspositionTableEntry(board.getHashkey(), depth, bestScore, HFEXACT, bestMove);
	else
		storeTranspositionTableEntry(board.getHashkey(), depth, alpha, HFALPHA, bestMove);
	return alpha;
}

static uShort generatePvLine(Board& board, Move moves[], short depth) {
	TranspositionTableEntry* t = retrieveTranspositionTableEntry(board.getHashkey());
	int count = 0;
	Move move;

	if (t != nullptr)
		move = t->move;

	while (move.isEmpty() == false && count < depth) {
		if (moveGenerator::moveExists(board, move)) {
			makeMove(board, move);
			moves[count++] = move;
		} else
			break;

		t = retrieveTranspositionTableEntry(board.getHashkey());
		if (t == nullptr)
			move();
		else
			move = t->move;
	}
	while (board.getPly() > 0)
		makeUndo(board);

	return count;
}

void searchPosition(Board& board) {
	clearSearch();
	board.ply = 0U;

	Move bestMove;
	Move pvLine[MAX_DEPTH];
	uShort pvSize;
	int score;

	for (short d = 1; d <= info.depth; ++d) {
		score = alphaBeta(board, d, -INFINIT, INFINIT);

		if (info.stop)
			d = info.depth;

		pvSize = generatePvLine(board, pvLine, d);
		bestMove = pvLine[0];

		std::cout << "info score " << score << " depth " << d << " nodes " << info.nodes << std::endl;
		std::cout << "pvLine: ";
		for (uShort i = 0U; i < pvSize; ++i)
			std::cout << pvLine[i] << " ";

		std::cout << std::endl;
	}
	std::cout << "bestmove: " << pvLine[0] << std::endl;
}