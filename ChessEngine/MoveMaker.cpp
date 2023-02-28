#include "Attacks.h"

/* This function removes the captured pawn from the bitBoard */
static void checkEnPassantCaptured(Board& board, const Move& move, const Color color, const bool isDoMove) {
	if (move.isEnPassantCapture()) {
		const Square captured = color == WHITE ? move.getTo() - 8U : move.getTo() + 8U;
		if (isDoMove)
			board.unsetPieceOnSquare(PAWN, ~color, captured);
		else
			board.setPieceOnSquare(PAWN, ~color, captured);
	}
}

/* This function replaces the pawn for the promotion piece */
static void checkPawnPromotion(Board& board, const Move& move, const Color color, const bool isDoMove) {
	if (move.isPawnPromotion())
		if (isDoMove) {
			board.unsetPieceOnSquare(PAWN, color, move.getTo());
			board.setPieceOnSquare(move.getPromotionPiece(), color, move.getTo());
		} else {
			board.unsetPieceOnSquare(move.getPromotionPiece(), color, move.getTo());
			board.setPieceOnSquare(PAWN, color, move.getTo());
		}
}

/* This function move the Rook to castle position */
static void checkCastleMove(Board& board, const Move& move, const Color color, const bool isDoMove) {
	if (move.isCastle()) {
		Square from, to;
		if (move.getTo() > move.getFrom()) {//king side
			if (isDoMove) {
				from = move.getFrom() + 3U;
				to = move.getFrom() + 1U;
			} else {
				from = move.getFrom() + 1U;
				to = move.getFrom() + 3U;
			}
		} else {//queen side
			if (isDoMove) {
				from = move.getFrom() - 4U;
				to = move.getFrom() - 1U;
			} else {
				from = move.getFrom() - 1U;
				to = move.getFrom() - 4U;
			}
		}
		board.unsetPieceOnSquare(ROOK, color, from);
		board.setPieceOnSquare(ROOK, color, to);
	}
}

/* This function removes the castle permission */
static void checkCastlePermission(Board& board, const Square square) {
	switch (square) {
	case A1:
		board.unsetCastlePermission(WHITE_QUEEN_CASTLE);
		break;
	case E1:
		board.unsetCastlePermission(WHITE_KING_CASTLE | WHITE_QUEEN_CASTLE);
		break;
	case H1:
		board.unsetCastlePermission(WHITE_KING_CASTLE);
		break;
	case A8:
		board.unsetCastlePermission(BLACK_QUEEN_CASTLE);
		break;
	case E8:
		board.unsetCastlePermission(BLACK_KING_CASTLE | BLACK_QUEEN_CASTLE);
		break;
	case H8:
		board.unsetCastlePermission(BLACK_KING_CASTLE);
	}
}

void makeUndo(Board& board) {
	if (board.historyCount == 0)
		return;

	Move move = board.history[--board.historyCount].move;

	board.whiteTime = !board.whiteTime;
	board.key ^= board.hashKeys.sideKey;
	const Color color = board.getColorTime();

	checkPawnPromotion(board, move, color, false);//this must be the first thing to do

	const Piece piece = board.getPieceFromSquare(color, move.getTo());

	if (piece == PAWN)
		checkEnPassantCaptured(board, move, color, false);
	else if (piece == KING)
		checkCastleMove(board, move, color, false);
	//Check castle permission must be done every move time
	board.setCastlePermission(board.history[board.historyCount].getCastlePermission());

	//set or clear en passant square
	board.setEnPassantSquare(board.history[board.historyCount].getEnPassantSquare());

	board.unsetPieceOnSquare(piece, color, move.getTo());
	board.setPieceOnSquare(piece, color, move.getFrom());

	if (move.isCapture()) //if is a capture move than put the piece back			
		board.setPieceOnSquare(move.getCaptured(), ~color, move.getTo());

	if (!board.whiteTime)
		--board.ply;

	board.fiftyMove = board.history[board.historyCount].fiftyMove;

	if (board.key != board.history[board.historyCount].key) {
		std::cout << "HASHKEY IS INVALID!" << std::endl;
		printBits("Board.flags:", board.flags);
		printBits("History.flags:", board.history[board.historyCount].flags);
		board.flags = board.history[board.historyCount].flags;
		board.key = board.history[board.historyCount].key;
		/* print the history to know the moves sequence */
		for (uShort i = 0; i < board.historyCount + 1; ++i)
			std::cout << board.history[i].move << " ";
		std::cout << std::endl;
		exit(EXIT_FAILURE);
	}
}

bool makeMove(Board& board, const Move& move) {
	if (move.isEmpty())
		return false;

	const Color color = board.getColorTime();

	if (move.getColor() != color)
		return false;

	const Piece piece = board.getPieceFromSquare(color, move.getFrom());

	if (piece == NONE_PIECE)
		return false;

	//0=======================SAVE=CURRENT=STATE===========================0
	board.history[board.historyCount].move = move;
	board.history[board.historyCount].flags = board.flags;
	board.history[board.historyCount].fiftyMove = board.fiftyMove;
	board.history[board.historyCount++].key = board.key;
	//0=====================================================================0

	++board.fiftyMove;
	if (move.isCapture()) {//if is a capture move then 
		board.fiftyMove = 0U;//reset fifty move counter
		board.unsetPieceOnSquare(move.getCaptured(), ~color, move.getTo());//remove the captured piece
	}

	board.unsetPieceOnSquare(piece, color, move.getFrom());
	board.setPieceOnSquare(piece, color, move.getTo());

	if (move.isPawnStart())//if is pawn start then set the  en passant square
		board.setEnPassantSquare(color == WHITE ? move.getTo() - 8U : move.getTo() + 8U);
	else if (board.getEnPassantSquare() != NONE_SQUARE)//clear the en passant square
		board.setEnPassantSquare(NONE_SQUARE);

	if (piece == PAWN) {//if si pawn move then
		board.fiftyMove = 0U;//reset fifty move counter
		checkEnPassantCaptured(board, move, color, true);
		checkPawnPromotion(board, move, color, true);
	} else if (piece == KING)
		checkCastleMove(board, move, color, true);

	checkCastlePermission(board, move.getFrom());
	checkCastlePermission(board, move.getTo());

	if (board.isBlackTime())
		++board.ply;

	board.whiteTime = !board.whiteTime;
	board.key ^= board.hashKeys.sideKey;

	if (attacks::isSquareAttacked(board, ~color, getFirstSquareOf(board.bitMaps[KING][color]))) {
		makeUndo(board);
		return false;
	}
	return true;
}