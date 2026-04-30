#include "Evaluator.h"

static const int PIECE_VALUE[] = { 10,30,30,50,90,900,0 };

/* This function set a score to a move and sort the list */
void sortMoves(const Board& board, Move moves[], const uShort movesCount) {
	for (Move* move = moves; move != moves + movesCount; ++move) {
		if (move->isCapture())
			move->score += 10 * PIECE_VALUE[move->getCaptured()] - (PIECE_VALUE[board.getPieceFromSquare(move->getColor(), move->getFrom())]);
		else if (moves->isEnPassantCapture())
			move->score += 10 * PIECE_VALUE[PAWN];
		else if (moves->isPawnPromotion())
			move->score += PIECE_VALUE[move->getPromotionPiece()];
		if (attacks::isSquareAttacked(board, ~move->getColor(), move->getTo()))
			move->score -= PIECE_VALUE[board.getPieceFromSquare(move->getColor(), move->getFrom())];
	}

	std::sort(moves, moves + movesCount, [](const Move& left, const Move& right) -> int {
		return std::cmp_less_equal<int, int>(left.score, right.score); });
}

static const int SQUARE_VALUE[6][64] = {
		{// PAWN
			0,  0,  0,  0,  0,  0,  0,  0,
			50, 50, 50, 50, 50, 50, 50, 50,
			10, 10, 20, 30, 30, 20, 10, 10,
			5,  5, 10, 25, 25, 10,  5,  5,
			0,  0,  0, 20, 20,  0,  0,  0,
			5, -5,-10,  0,  0,-10, -5,  5,
			5, 10, 10,-20,-20, 10, 10,  5,
			0,  0,  0,  0,  0,  0,  0,  0},
		{// kNIGHT
			-50,-40,-30,-30,-30,-30,-40,-50,
			-40,-20,  0,  0,  0,  0,-20,-40,
			-30,  0, 10, 15, 15, 10,  0,-30,
			-30,  5, 15, 20, 20, 15,  5,-30,
			-30,  0, 15, 20, 20, 15,  0,-30,
			-30,  5, 10, 15, 15, 10,  5,-30,
			-40,-20,  0,  5,  5,  0,-20,-40,
			-50,-40,-30,-30,-30,-30,-40,-50},
		{// BISHOP
			-20,-10,-10,-10,-10,-10,-10,-20,
			-10,  0,  0,  0,  0,  0,  0,-10,
			-10,  0,  5, 10, 10,  5,  0,-10,
			-10,  5,  5, 10, 10,  5,  5,-10,
			-10,  0, 10, 10, 10, 10,  0,-10,
			-10, 10, 10, 10, 10, 10, 10,-10,
			-10,  5,  0,  0,  0,  0,  5,-10,
			-20,-10,-10,-10,-10,-10,-10,-20},
		{// ROOK
			 0,  0,  0,  0,  0,  0,  0,  0,
			 5, 10, 10, 10, 10, 10, 10,  5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			 0,  0,  0,  5,  5,  0,  0,  0},
		{// QUEEN
			-20,-10,-10, -5, -5,-10,-10,-20,
			-10,  0,  0,  0,  0,  0,  0,-10,
			-10,  0,  5,  5,  5,  5,  0,-10,
			-5,  0,  5,  5,  5,  5,  0, -5,
			 0,  0,  5,  5,  5,  5,  0, -5,
			-10,  5,  5,  5,  5,  5,  0,-10,
			-10,  0,  5,  0,  0,  0,  0,-10,
			-20,-10,-10, -5, -5,-10,-10,-20},
		{// KING
			-30,-40,-40,-50,-50,-40,-40,-30,
			-30,-40,-40,-50,-50,-40,-40,-30,
			-30,-40,-40,-50,-50,-40,-40,-30,
			-30,-40,-40,-50,-50,-40,-40,-30,
			-20,-30,-30,-40,-40,-30,-30,-20,
			-10,-20,-20,-20,-20,-20,-20,-10,
			 20, 20,  0,  0,  0,  0, 20, 20,
			 20, 30, 10,  0,  0, 10, 30, 20} };

static void evaluatePosition(const Board& board, const Color color, int& materialWeight, int& numPieces) {
	BitBoard pieceBitBoard;
	Square s;
	Square(*popSquareOf)(BitBoard&);
	if (color == WHITE)
		popSquareOf = popFirstSquareOf;
	else
		popSquareOf = popLastSquareOf;

	for (Piece p = PAWN; p != NONE_PIECE; ++p) {
		pieceBitBoard = board.getBitBoardOf(p, color);
		while (pieceBitBoard) {
			s = popSquareOf(pieceBitBoard);
			if (board.isWhiteTime())
				s = ~s;

			materialWeight += PIECE_VALUE[p];
			materialWeight += SQUARE_VALUE[p][s];

			++numPieces;
		}
	}
}

int evaluatePosition(const Board& board) {
	int whiteMaterialWeight = 0;
	int blackMaterialWeight = whiteMaterialWeight;
	int numWhitePieces = blackMaterialWeight;
	int numBlackPieces = numWhitePieces;

	evaluatePosition(board, WHITE, whiteMaterialWeight, numWhitePieces);
	evaluatePosition(board, BLACK, blackMaterialWeight, numBlackPieces);

	int who2Move = board.isWhiteTime() ? 1 : -1;
	return (whiteMaterialWeight - blackMaterialWeight) * (numWhitePieces - numBlackPieces) * who2Move;
}