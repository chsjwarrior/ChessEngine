#include "Attacks.h"

/* This function remove the captured pawn of the bitBoard */
static void checkEnPassantCaptured(BitBoard& bitBoard, const Move& move, const Color color, const bool isDoMove) {
	if (move.isEnPassantCapture()) {
		const Square captured = color == WHITE ? move.getTo() - 8U : move.getTo() + 8U;
		if (isDoMove)
			bitBoard.unsetPieceOnSquare(PAWN, ~color, captured);
		else
			bitBoard.setPieceOnSquare(PAWN, ~color, captured);
	}
}

/* This function replace the pawn for the promotion piece */
static void checkPawnPromotion(BitBoard& bitBoard, const Move& move, const Color color, const bool isDoMove) {
	if (move.isPawnPromotion())
		if (isDoMove) {
			bitBoard.unsetPieceOnSquare(PAWN, color, move.getTo());
			bitBoard.setPieceOnSquare(move.getPromotionPiece(), color, move.getTo());
		} else {
			bitBoard.unsetPieceOnSquare(move.getPromotionPiece(), color, move.getTo());
			bitBoard.setPieceOnSquare(PAWN, color, move.getTo());
		}
}

/* This function move the Rook to castle position */
static void checkCastleMove(BitBoard& bitBoard, const Move& move, const Color color, const bool isDoMove) {
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
		bitBoard.unsetPieceOnSquare(ROOK, color, from);
		bitBoard.setPieceOnSquare(ROOK, color, to);
	}
}

/* This function check the castle permission */
static void checkCastlePermission(BitBoard& bitBoard, const Square square) {
	switch (square) {
	case A1:
		bitBoard.unsetCastlePermission(WHITE_QUEEN_CASTLE);
		break;
	case E1:
		bitBoard.unsetCastlePermission(WHITE_KING_CASTLE | WHITE_QUEEN_CASTLE);
		break;
	case H1:
		bitBoard.unsetCastlePermission(WHITE_KING_CASTLE);
		break;
	case A8:
		bitBoard.unsetCastlePermission(BLACK_QUEEN_CASTLE);
		break;
	case E8:
		bitBoard.unsetCastlePermission(BLACK_KING_CASTLE | BLACK_QUEEN_CASTLE);
		break;
	case H8:
		bitBoard.unsetCastlePermission(BLACK_KING_CASTLE);
	}
}

void makeUndo(BitBoard& bitBoard) {
	if (bitBoard.historyCount == 0)
		return;

	Move move = bitBoard.history[--bitBoard.historyCount].move;

	bitBoard.whiteTime = !bitBoard.whiteTime;
	bitBoard.key ^= bitBoard.hashKeys.sideKey;
	const Color color = bitBoard.getColorTime();

	checkPawnPromotion(bitBoard, move, color, false);//this is the first thing to do

	const Piece piece = bitBoard.getPieceFromSquare(color, move.getTo());

	if (piece == PAWN)
		checkEnPassantCaptured(bitBoard, move, color, false);
	else if (piece == KING) {
		checkCastleMove(bitBoard, move, color, false);
		bitBoard.setCastlePermission(bitBoard.history[bitBoard.historyCount].getCastlePermission());
	} else
		bitBoard.setCastlePermission(bitBoard.history[bitBoard.historyCount].getCastlePermission());

	//set or clear en passant square
	bitBoard.setEnPassantSquare(bitBoard.history[bitBoard.historyCount].getEnPassantSquare());

	bitBoard.unsetPieceOnSquare(piece, color, move.getTo());
	bitBoard.setPieceOnSquare(piece, color, move.getFrom());

	if (move.isCapture()) //if is capture move than put the piece back			
		bitBoard.setPieceOnSquare(move.getCaptured(), ~color, move.getTo());

	if (!bitBoard.whiteTime)
		--bitBoard.ply;

	bitBoard.fiftyMove = bitBoard.history[bitBoard.historyCount].fiftyMove;

	if (bitBoard.key != bitBoard.history[bitBoard.historyCount].key) {
		std::cout << "HASHKEY IS INVALID!" << std::endl;
		printBits("BitBoard.flags:", bitBoard.flags);
		printBits("History.flags:", bitBoard.history[bitBoard.historyCount].flags);
		bitBoard.flags = bitBoard.history[bitBoard.historyCount].flags;
		bitBoard.key = bitBoard.history[bitBoard.historyCount].key;
		//exit(EXIT_FAILURE);
	}
}

bool makeMove(BitBoard& bitBoard, const Move& move) {
	if (move.isEmpty())
		return false;

	const Color color = bitBoard.getColorTime();

	if (move.getColor() != color)
		return false;

	const Piece piece = bitBoard.getPieceFromSquare(color, move.getFrom());

	if (piece == NONE_PIECE)
		return false;

	//0=======================SAVE=CURRENT=STATE===========================0
	bitBoard.history[bitBoard.historyCount].move = move;
	bitBoard.history[bitBoard.historyCount].flags = bitBoard.flags;
	bitBoard.history[bitBoard.historyCount].fiftyMove = bitBoard.fiftyMove;
	bitBoard.history[bitBoard.historyCount++].key = bitBoard.key;
	//0=====================================================================0

	++bitBoard.fiftyMove;
	if (move.isCapture()) {//if is capture move than 
		bitBoard.fiftyMove = 0U;//reset fifty move counter
		bitBoard.unsetPieceOnSquare(move.getCaptured(), ~color, move.getTo());//remove captured piece
	}

	bitBoard.unsetPieceOnSquare(piece, color, move.getFrom());
	bitBoard.setPieceOnSquare(piece, color, move.getTo());

	if (move.isPawnStart())//if is pawn start set en passant square
		bitBoard.setEnPassantSquare(color == WHITE ? move.getTo() - 8U : move.getTo() + 8U);
	else if (bitBoard.getEnPassantSquare() != NONE_SQUARE)//clear en passant square
		bitBoard.setEnPassantSquare(NONE_SQUARE);

	if (piece == PAWN) {//if pawn move than
		bitBoard.fiftyMove = 0U;//reset fifty move counter
		checkEnPassantCaptured(bitBoard, move, color, true);
		checkPawnPromotion(bitBoard, move, color, true);
	} else if (piece == KING)
		checkCastleMove(bitBoard, move, color, true);

	checkCastlePermission(bitBoard, move.getFrom());
	checkCastlePermission(bitBoard, move.getTo());

	if (bitBoard.isBlackTime())
		++bitBoard.ply;

	bitBoard.whiteTime = !bitBoard.whiteTime;
	bitBoard.key ^= bitBoard.hashKeys.sideKey;

	if (attacks::isSquareAttacked(bitBoard, ~color, getFirstSquareOf(bitBoard.bitMaps[KING][color]))) {
		makeUndo(bitBoard);
		return false;
	}
	return true;
}