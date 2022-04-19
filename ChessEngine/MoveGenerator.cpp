#include "MoveGenerator.h"

static Bitmap friendPieces;
static Bitmap enemyPieces;
static uShort movesCount;

/* This function tests if is possible to make King castle */
static bool canMakeKingCastle(const BitBoard& bitBoard, const Color color) {
	if (bitBoard.hasCastlePermission(KING_CASTLE, color))
		if (((bitBoard.getBitmapPiece(KING, color) << 1U | bitBoard.getBitmapPiece(KING, color) << 2U) & (friendPieces | enemyPieces)) == 0) { //isCastlePathClear
			const Rank relativeRank = color == WHITE ? RANK_1 : RANK_8;
			if (!attacks::isSquareAttacked(bitBoard, ~color, getSquareOf(FILE_F, relativeRank)))//isCastlePathSecury
				return !attacks::isSquareAttacked(bitBoard, ~color, getSquareOf(FILE_G, relativeRank));
		}
	return false;
}
/* This function tests if is possible to make Queen castle */
static bool canMakeQueenCastle(const BitBoard& bitBoard, const Color color) {
	if (bitBoard.hasCastlePermission(QUEEN_CASTLE, color))
		if (((bitBoard.getBitmapPiece(KING, color) >> 1U | bitBoard.getBitmapPiece(KING, color) >> 2U | bitBoard.getBitmapPiece(KING, color) >> 3U) & (friendPieces | enemyPieces)) == 0) {//isCastlePathClear
			const Rank relativeRank = color == WHITE ? RANK_1 : RANK_8;
			if (!attacks::isSquareAttacked(bitBoard, ~color, getSquareOf(FILE_C, relativeRank)))//isCastlePathSecury
				return !attacks::isSquareAttacked(bitBoard, ~color, getSquareOf(FILE_D, relativeRank));//isCastlePathSecury
		}
	return false;
}
/* This function sets the captured piece in the Move if any */
static void setCapture(const BitBoard& bitBoard, Move& move, const Color color, const Square to) {
	const Piece capture = bitBoard.getPieceFromSquare(~color, to);
	move.setCaptured(capture);
	move.score += PIECE_VALUE[capture];
}
/* This function returns a bitmap with the attack squares */
static Bitmap getPiecesMoves(const BitBoard& bitBoard, const Piece piece, const Color color, const Square square) {
	Bitmap attacks = 0UL;
	if (piece == KING)
		attacks = attacks::getKingMoves(getBitmapOf(square));
	else if (piece == KNIGHT)
		attacks = attacks::getKnightMoves(getBitmapOf(square));
	else {
		const Bitmap allPieces = friendPieces | enemyPieces;

		if (piece == PAWN)
			attacks = attacks::getPawnMoves(allPieces, color, getBitmapOf(bitBoard.getEnPassantSquare()), getBitmapOf(square));
		else {
			const File file = getFileOf(square);
			const Rank rank = getRankOf(square);

			if (piece == BISHOP)
				attacks = attacks::getBishopMoves(allPieces, file, rank, getBitmapOf(square));
			else if (piece == ROOK)
				attacks = attacks::getRookMoves(allPieces, file, rank, getBitmapOf(square));
			else if (piece == QUEEN)
				attacks = attacks::getBishopMoves(allPieces, file, rank, getBitmapOf(square)) | attacks::getRookMoves(allPieces, file, rank, getBitmapOf(square));
		}
	}
	return attacks & ~friendPieces;//remove friend pieces square
}
/* This function populates an array with the moves and returns the size */
static void catalogMoves(const BitBoard& bitBoard, Move moves[], const Piece piece, const Color color, const Square from, Bitmap attacks) {
	Square to;
	Move move;
	Square(*popSquareOf)(Bitmap&) = popFirstSquareOf;
	if (color == BLACK)
		popSquareOf = popLastSquareOf;

	if (piece == PAWN) {
		while (attacks) {
			to = popSquareOf(attacks);

			move();
			move.setFrom(from);
			move.setTo(to);
			move.setColor(color);

			if (color == WHITE && getRankOf(to) == RANK_8 ||
				color == BLACK && getRankOf(to) == RANK_1) {
				setCapture(bitBoard, move, color, to);

				for (Piece p = QUEEN; p > PAWN; --p) {
					move.setPromotionPiece(p);
					moves[movesCount] = move;
					moves[movesCount++].score = PIECE_VALUE[p];
				}
				continue;
			}
			if (color == WHITE && move.getTo() - move.getFrom() == 16 || color == BLACK && move.getFrom() - move.getTo() == 16)
				move.setPawnStart();
			else if (to == bitBoard.getEnPassantSquare())
				move.setEnPassantCapture();
			else
				setCapture(bitBoard, move, color, to);

			moves[movesCount++] = move;
		}
	} else {
		while (attacks) {
			to = popSquareOf(attacks);

			move();
			move.setFrom(from);
			move.setTo(to);
			move.setColor(color);
			setCapture(bitBoard, move, color, to);

			moves[movesCount++] = move;
		}
		move();
		if (piece == KING)
			if (!attacks::isSquareAttacked(bitBoard, ~color, getFirstSquareOf(bitBoard.getBitmapPiece(KING, color)))) {//King is not in check
				if (canMakeKingCastle(bitBoard, color)) {
					move.setFrom(from);
					move.setTo(from + 2U);
					move.setColor(color);
					move.setCastle();
					moves[movesCount++] = move;
				}
				if (canMakeQueenCastle(bitBoard, color)) {
					move.setFrom(from);
					move.setTo(from - 2U);
					move.setColor(color);
					move.setCastle();
					moves[movesCount++] = move;
				}
			}
	}
}

uShort moveGenerator::generateMoves(const BitBoard& bitBoard, Move moves[], const Square square) {
	const Color color = bitBoard.getColorTime();
	const Piece piece = bitBoard.getPieceFromSquare(color, square);

	if (piece == NONE_PIECE)
		return 0U;

	friendPieces = bitBoard.getBitmapAllPieces(color);
	enemyPieces = bitBoard.getBitmapAllPieces(~color);
	movesCount = 0U;

	Bitmap attacks = getPiecesMoves(bitBoard, piece, color, square);
	catalogMoves(bitBoard, moves, piece, color, square, attacks);
	return movesCount;
}

uShort moveGenerator::generateMoves(const BitBoard& bitBoard, Move moves[]) {
	const Color color = bitBoard.getColorTime();
	friendPieces = bitBoard.getBitmapAllPieces(color);
	enemyPieces = bitBoard.getBitmapAllPieces(~color);
	movesCount = 0U;

	Bitmap attacks;
	Bitmap pieceBitmap;
	Square square;
	Square(*popSquareOf)(Bitmap&) = popFirstSquareOf;
	if (color == BLACK)
		popSquareOf = popLastSquareOf;

	for (Piece p = PAWN; p != NONE_PIECE; ++p) {
		pieceBitmap = bitBoard.getBitmapPiece(p, color);
		while (pieceBitmap) {
			square = popSquareOf(pieceBitmap);
			attacks = getPiecesMoves(bitBoard, p, color, square);
			catalogMoves(bitBoard, moves, p, color, square, attacks);
		}
	}
	return movesCount;
}

uShort moveGenerator::generateCaptureMoves(const BitBoard& bitBoard, Move moves[]) {
	generateMoves(bitBoard, moves);

	for (short i = 0; i < movesCount; ++i)
		if (moves[i].getCaptured() == NONE_PIECE && !moves[i].isEnPassantCapture()) {
			moves[i]();
			std::swap(moves[i--], moves[--movesCount]);
		}

	return movesCount;
}