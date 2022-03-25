#include "Chess.h"

static void checkUp() {
	auto now = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
	info.stop = info.stopTime > 0 && duration.count() > info.stopTime;
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

static int evaluatePosition(const BitBoard& bitBoard) {
	Bitmap pieceBitmap;
	Square s = NONE_SQUARE;
	int materialWeight = 0;
	int numWhitePieces = materialWeight;
	int numBlackPieces = numWhitePieces;
	const Square(*popSquareOf)(Bitmap&) = nullptr;
	Move moves[MAX_MOVES];

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
					materialWeight += PIECE_VALUE[p];
					s = ~s;
				} else
					materialWeight += -PIECE_VALUE[p];

				materialWeight += SQUARE_VALUE[p][s];

				if (c == WHITE)
					++numWhitePieces;
				else if (c == BLACK)
					++numBlackPieces;
			}
		}
	}
	int who2Move = bitBoard.isWhiteTime() ? 1 : -1;
	return materialWeight * (numWhitePieces - numBlackPieces) * who2Move;
}

struct Line {
	int count;
	Move moves[MAX_DEPTH];// The line.
};

static int negaMax(BitBoard& bitBoard, short depth, int alpha, int beta, Line* pvLine) {
	if (depth <= 0) {
		pvLine->count = 0;
		return evaluatePosition(bitBoard);
	}

	info.nodes++;

	if (info.nodes >= 2047)
		checkUp();

	if (bitBoard.isRepetition() || bitBoard.getFiftyMove() >= 100 && bitBoard.getPly())
		return 0;

	if (bitBoard.getPly() > MAX_DEPTH - 1) 		
		return evaluatePosition(bitBoard);

	const Color color = bitBoard.getColorTime();
	const bool inCheck = attacks::isSquareAttacked(bitBoard, ~color, getFirstSquareOf(bitBoard.getBitmapPiece(KING, color)));

	if (inCheck)
		++depth;

	uInt legal = 0U;
	int score = -INFINIT;

	Move moves[MAX_MOVES];
	uShort moveCount = moveGenerator::generateMoves(bitBoard, moves);
	MoveMaker& moveMaker = MoveMaker::getInstance();
	Line line;

	for (uShort i = 0; i < moveCount; ++i) {
		swapForBestMove(i, moves, moveCount);

		if (!moveMaker.makeMove(bitBoard, moves[i]))
			continue;

		++legal;
		score = -negaMax(bitBoard, depth - 1, -beta, -alpha, &line);
		moveMaker.makeUndo(bitBoard);

		if (info.stop == true)
			return 0;

		if (score >= beta)
			return beta;

		if (score > alpha) {
			alpha = score;
			pvLine->moves[0] = moves[i];
			memcpy(pvLine->moves + 1, line.moves, line.count * sizeof(Move));
			pvLine->count = line.count + 1;
		}
	}

	if (legal == 0) {
		if (inCheck)
			return -INFINIT + bitBoard.getPly();
		return 0;
	}
	return alpha;
}

void searchPosition(BitBoard& bitBoard) {
	info.nodes = 0UL;
	info.stop = false;

	int score = -INFINIT;
	Line line;

	for (short d = 1; d <= info.depth; ++d) {
		score = negaMax(bitBoard, d - 1, -INFINIT, INFINIT, &line);
		if (info.stop)
			d = info.depth;

		std::cout << "info score " << score << " depth " << d << " nodes " << info.nodes;
		//std::cout << " time " << getTimeMilliseconds() - info.startTime << std::endl;
		for (Move* move = line.moves; move != line.moves + line.count; ++move)
			std::cout << " " << *move << " ";
		std::cout << std::endl;
	}

	//std::cout << "bestmove: " << bestMove << std::endl;
}