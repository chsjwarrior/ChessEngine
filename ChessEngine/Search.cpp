#include "Chess.h"

static void checkUp() {
	//if (info.stopTime > 0 && getTimeMilliseconds() > info.stopTime)
		//info.stop = true;
}

static void swapForBestMove(const uShort index, Move moves[], const uShort size) {
	int bestScore = -30000;
	int bestIndex = 0;

	for (uShort i = index; i < size; ++i)
		if (moves[i].score > bestScore) {
			bestScore = moves[i].score;
			bestIndex = i;
		}

	std::swap(moves[index], moves[bestIndex]);
}

struct Line {
	uInt count = 0U;
	Move moves[MAX_MOVES];
};

static int evaluatePosition(const BitBoard& bitBoard) {
	Bitmap pieceBitmap;
	int score = 0;
	Square s = NONE_SQUARE;
	const Square(*popSquareOf)(Bitmap&) = nullptr;

	for (uChar c = WHITE; c <= BLACK; ++c) {
		if (c == WHITE)
			popSquareOf = popFirstSquareOf;
		else if (c == BLACK)
			popSquareOf = popLastSquareOf;
		for (Piece p = PAWN; p != NONE_PIECE; ++p) {
			pieceBitmap = bitBoard.getBitmapPiece(p, static_cast<Color>(c));
			while (pieceBitmap) {
				s = popSquareOf(pieceBitmap);

				if (bitBoard.isBlackTime()) {
					score += PIECE_VALUE[p];
					s = ~s;
				} else
					score += -PIECE_VALUE[p];

				score += SQUARE_VALUE[p][s];
			}
		}
	}
	return score;
}

static int quiescence(BitBoard& bitBoard, int alpha, int beta) {
	++info.nodes;
	//if (info.nodes >= 2047)
		//checkUp();

	//if (bitBoard.isRepetition() || bitBoard.getFiftyMove() >= 100)
		//return 0;

	//if (bitBoard.getPly() > MAX_DEPTH - 1)
		//return evaluatePosition(bitBoard);

	int score = evaluatePosition(bitBoard);

	if (score >= beta)
		return beta;

	if (score > alpha)
		alpha = score;

	Move moves[MAX_MOVES];
	uShort movesCount = moveGenerator::generateCaptureMoves(bitBoard, moves);
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

static int negaMax(BitBoard& bitBoard, short depth, int alpha, int beta, Line& pLine) {
	if (depth <= 0)
		return quiescence(bitBoard, alpha, beta);

	info.nodes++;

	//if (info.nodes >= 2047)
		//checkUp();

	//if (bitBoard.isRepetition() || bitBoard.getFiftyMove() >= 100 && bitBoard.getPly())
		//return 0;

	if (bitBoard.getPly() > MAX_DEPTH - 1)
		return evaluatePosition(bitBoard);

	const Color color = bitBoard.getColorTime();
	const bool inCheck = attacks::isSquareAttacked(bitBoard, ~color, getFirstSquareOf(bitBoard.getBitmapPiece(KING, color)));

	if (inCheck)
		++depth;

	Line line;
	uInt legal = 0U;
	int score = -30000;

	Move moves[MAX_MOVES];
	uShort moveCount = moveGenerator::generateMoves(bitBoard, moves);
	MoveMaker& moveMaker = MoveMaker::getInstance();

	for (uShort i = 0; i < moveCount; ++i) {
		swapForBestMove(i, moves, moveCount);

		if (!moveMaker.makeMove(bitBoard, moves[i]))
			continue;

		legal++;
		score = -negaMax(bitBoard, depth - 1, -beta, -alpha, line);
		moveMaker.makeUndo(bitBoard);

		//if (info.stop == true)
			//return 0;

		if (score >= beta)
			return beta;

		if (score > alpha) {
			alpha = score;
			pLine.moves[0] = moves[i];
			memcpy(pLine.moves + 1, line.moves, line.count * sizeof(Move));
			pLine.count = line.count + 1;
		}
	}

	if (legal == 0) {
		if (inCheck)
			return -30000 + bitBoard.getPly();
		return 0;
	}
	return alpha;
}

void searchPosition(BitBoard& bitBoard) {
	info.nodes = 0UL;
	info.stop = false;

	Line line;
	int score = -30000;

	for (short d = 1; d <= info.depth; ++d) {
		score = negaMax(bitBoard, d - 1, -30000, 30000, line);
		//if (info.stop)
			//break;

		std::cout << "info score " << score << " depth " << d << " nodes " << info.nodes;
		//std::cout << " time " << getTimeMilliseconds() - info.startTime << std::endl;
		for (Move* move = line.moves; move != line.moves + line.count; ++move)
			std::cout << " " << *move << " ";
		std::cout << std::endl;
	}

	//std::cout << "bestmove: " << bestMove << std::endl;
}