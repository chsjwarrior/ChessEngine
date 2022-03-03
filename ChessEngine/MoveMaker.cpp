#include "MoveMaker.h"

MoveMaker& MoveMaker::getInstance() {
	static MoveMaker instance;
	return instance;
}

void MoveMaker::checkCapture(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const {
	const Piece captured = move.getCaptured();
	if (captured != NONE_PIECE)
		if (isNotUndo) {
			bitBoard.fiftyMove = 0U;
			bitBoard.unsetPieceOnSquare(captured, ~color, move.getTo());
		} else
			bitBoard.setPieceOnSquare(captured, ~color, move.getTo());
}

void MoveMaker::checkEnPassant(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const {
	Square enPassantSquare = NONE_SQUARE;
	if (isNotUndo) {
		if (move.isPawnStart())
			if (color == WHITE)
				enPassantSquare = move.getTo() - 8U;
			else if (color == BLACK)
				enPassantSquare = move.getTo() + 8U;
	} else
		enPassantSquare = bitBoard.history[bitBoard.historyCount].getEnPassantSquare();
	bitBoard.setEnPassantSquare(enPassantSquare);
}

void MoveMaker::checkEnPassantCaptured(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const {
	if (move.isEnPassantCapture()) {
		Square captured = NONE_SQUARE;
		if (color == WHITE)
			captured = move.getTo() - 8U;
		else if (color == BLACK)
			captured = move.getTo() + 8U;
		if (isNotUndo)
			bitBoard.unsetPieceOnSquare(PAWN, ~color, captured);
		else
			bitBoard.setPieceOnSquare(PAWN, ~color, captured);
	}
}

void MoveMaker::checkPawnPromotion(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const {
	if (move.isPawnPromotion()) {
		const Piece promotion = move.getPromotionPiece();
		if (isNotUndo) {
			bitBoard.unsetPieceOnSquare(PAWN, color, move.getTo());
			bitBoard.setPieceOnSquare(promotion, color, move.getTo());
		} else {
			bitBoard.unsetPieceOnSquare(promotion, color, move.getTo());
			bitBoard.setPieceOnSquare(PAWN, color, move.getTo());
		}
	}
}

void MoveMaker::checkCastleMove(BitBoard& bitBoard, const Move& move, const Color color, const bool isNotUndo) const {
	if (move.isKingCastle()) {
		Square from, to;
		if (isNotUndo) {
			from = move.getFrom() + 3U;
			to = move.getFrom() + 1U;
		} else {
			from = move.getFrom() + 1U;
			to = move.getFrom() + 3U;
		}
		bitBoard.unsetPieceOnSquare(ROOK, color, from);
		bitBoard.setPieceOnSquare(ROOK, color, to);
	} else if (move.isQueenCastle()) {
		Square from, to;
		if (isNotUndo) {
			from = move.getFrom() - 4U;
			to = move.getFrom() - 1U;
		} else {
			from = move.getFrom() - 1U;
			to = move.getFrom() - 4U;
		}
		bitBoard.unsetPieceOnSquare(ROOK, color, from);
		bitBoard.setPieceOnSquare(ROOK, color, to);
	}
}

void MoveMaker::makeUndo(BitBoard& bitBoard) const {
	if (bitBoard.historyCount == 0)
		return;

	Move move = bitBoard.history[--bitBoard.historyCount].move;

	bitBoard.whiteTime = !bitBoard.whiteTime;
	bitBoard.key ^= bitBoard.hashKeys.sideKey;
	const Color color = bitBoard.getColorTime();

	checkPawnPromotion(bitBoard, move, color, false);//this is the first thing to do

	const Piece piece = bitBoard.getPieceFromSquare(color, move.getTo());

	if (piece == ROOK) {
		bitBoard.setCastlePermission(KING_CASTLE, color, bitBoard.history[bitBoard.historyCount].hasCastlePermission(KING_CASTLE, color));
		bitBoard.setCastlePermission(QUEEN_CASTLE, color, bitBoard.history[bitBoard.historyCount].hasCastlePermission(QUEEN_CASTLE, color));
	} else if (piece == KING) {
		checkCastleMove(bitBoard, move, color, false);
		bitBoard.setCastlePermission(KING_CASTLE, color, bitBoard.history[bitBoard.historyCount].hasCastlePermission(KING_CASTLE, color));
		bitBoard.setCastlePermission(QUEEN_CASTLE, color, bitBoard.history[bitBoard.historyCount].hasCastlePermission(QUEEN_CASTLE, color));
	} else if (piece == PAWN)
		checkEnPassantCaptured(bitBoard, move, color, false);

	checkEnPassant(bitBoard, move, color, false);

	bitBoard.unsetPieceOnSquare(piece, color, move.getTo());
	bitBoard.setPieceOnSquare(piece, color, move.getFrom());

	checkCapture(bitBoard, move, color, false);

	if (!bitBoard.whiteTime)
		--bitBoard.ply;
	bitBoard.fiftyMove = bitBoard.history[bitBoard.historyCount].fiftyMove;
	if (bitBoard.key != bitBoard.history[bitBoard.historyCount].key) {
		std::cerr << "HASHKEY IS INVALID!" << std::endl;
		bitBoard.flags = bitBoard.history[bitBoard.historyCount].flags;
		bitBoard.key = bitBoard.history[bitBoard.historyCount].key;
		//exit(EXIT_FAILURE);
	}
}

bool MoveMaker::makeMove(BitBoard& bitBoard, const Move& move) const {
	if (move.isEmpty())
		return false;

	const Color color = bitBoard.getColorTime();

	if (move.getColor() != color)
		return false;

	const Piece piece = bitBoard.getPieceFromSquare(color, move.getFrom());

	if (piece == NONE_PIECE)
		return false;

	//0=======================SAVE=PREVIOUS=STATE===========================0
	bitBoard.history[bitBoard.historyCount].move = move;
	bitBoard.history[bitBoard.historyCount].flags = bitBoard.flags;
	bitBoard.history[bitBoard.historyCount].fiftyMove = bitBoard.fiftyMove;
	bitBoard.history[bitBoard.historyCount].key = bitBoard.key;
	//0=====================================================================0

	++bitBoard.fiftyMove;
	checkCapture(bitBoard, move, color, true);

	bitBoard.unsetPieceOnSquare(piece, color, move.getFrom());
	bitBoard.setPieceOnSquare(piece, color, move.getTo());

	checkEnPassant(bitBoard, move, color, true);

	if (piece == PAWN) {
		bitBoard.fiftyMove = 0U;
		checkEnPassantCaptured(bitBoard, move, color, true);
		checkPawnPromotion(bitBoard, move, color, true);
	} else if (bitBoard.hasCastlePermission(KING_CASTLE, color) ||
			   bitBoard.hasCastlePermission(QUEEN_CASTLE, color)) {
		if (piece == KING) {
			checkCastleMove(bitBoard, move, color, true);
			bitBoard.setCastlePermission(KING_CASTLE, color, false);
			bitBoard.setCastlePermission(QUEEN_CASTLE, color, false);
		} else if (piece == ROOK) {
			const Square from = move.getFrom();
			bitBoard.setCastlePermission(KING_CASTLE, color, !(from == H1 || from == H8));
			bitBoard.setCastlePermission(QUEEN_CASTLE, color, !(from == A1 || from == A8));
		}
	}

	if (bitBoard.isBlackTime())
		++bitBoard.ply;
	++bitBoard.historyCount;
	bitBoard.whiteTime = !bitBoard.whiteTime;
	bitBoard.key ^= bitBoard.hashKeys.sideKey;

	if (isSquareAttacked(bitBoard, ~color, getFirstSquareOf(bitBoard.bitMaps[KING][color]))) {
		makeUndo(bitBoard);
		return false;
	}
	return true;
}