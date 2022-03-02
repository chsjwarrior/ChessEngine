#include "MoveGenerator.h"

Bitmap friendPieces;
Bitmap enemyPieces;
uShort movesCount;

/* This function sets the captured piece in the Move if any */
void setCapture(const BitBoard& bitBoard, Move& move, const Color color, const Square to) {
	const Piece capture = bitBoard.getPieceFromSquare(~color, to);
	move.setCaptured(capture);
	move.score += PIECE_VALUE[capture];
}
/* This function tests if is possible to make King castle */
bool canMakeKingCastle(const BitBoard& bitBoard, const Color color) {
	if (bitBoard.hasCastlePermission(KING_CASTLE, color)) {
		const Rank relativeRank = color == WHITE ? RANK_1 : RANK_8;
		if (!hasIntersection((FILES[FILE_F] | FILES[FILE_G]) & RANKS[relativeRank], friendPieces | enemyPieces)) //isCastlePathClear
			if (!attacks::isSquareAttacked(bitBoard, ~color, getSquareOf(FILE_F, relativeRank)))//isCastlePathSecury
				return !attacks::isSquareAttacked(bitBoard, ~color, getSquareOf(FILE_G, relativeRank));
	}
	return false;
}
/* This function tests if is possible to make Queen castle */
bool canMakeQueenCastle(const BitBoard& bitBoard, const Color color) {
	if (bitBoard.hasCastlePermission(QUEEN_CASTLE, color)) {
		const Rank relativeRank = color == WHITE ? RANK_1 : RANK_8;
		if (!hasIntersection((FILES[FILE_B] | FILES[FILE_C] | FILES[FILE_D]) & RANKS[relativeRank], friendPieces | enemyPieces))//isCastlePathClear
			if (!attacks::isSquareAttacked(bitBoard, ~color, getSquareOf(FILE_C, relativeRank)))//isCastlePathSecury
				return !attacks::isSquareAttacked(bitBoard, ~color, getSquareOf(FILE_D, relativeRank));//isCastlePathSecury					
	}
	return false;
}
/* This function returns a bitmap with the attack squares */
Bitmap getPiecesMoves(const BitBoard& bitBoard, const Piece piece, const Color color, const Square square) {
	const Bitmap squareBitmap = getBitmapOf(square);//1UL << square;
	Bitmap attacks = 0UL;

	if (piece == KING)
		attacks = attacks::getKingMoves(squareBitmap);
	else if (piece == KNIGHT)
		attacks = attacks::getKnightMoves(squareBitmap);
	else {
		const Bitmap allPieces = getUnion(friendPieces, enemyPieces);

		if (piece == PAWN)
			attacks = attacks::getPawnMoves(allPieces, color, getBitmapOf(bitBoard.getEnPassantSquare()), squareBitmap);
		else {
			const File file = getFileOf(square);
			const Rank rank = getRankOf(square);

			if (piece == BISHOP)
				attacks = attacks::getBishopMoves(allPieces, file, rank, squareBitmap);
			else if (piece == ROOK)
				attacks = attacks::getRookMoves(allPieces, file, rank, squareBitmap);
			else if (piece == QUEEN)
				attacks = attacks::getBishopMoves(allPieces, file, rank, squareBitmap) | attacks::getRookMoves(allPieces, file, rank, squareBitmap);
		}
	}
	return unsetIntersections(attacks, bitBoard.getBitmapAllPieces(color));//remove friend pieces square
}
/* This function populates an array with the moves and returns the size */
void catalogMoves(const BitBoard& bitBoard, Move moves[], const Piece piece, const Color color, const Square from, Bitmap attacks) {
	Square to = NONE_SQUARE;
	Move move;
	const Square(*popSquareOf)(Bitmap&) = nullptr;

	if (color == WHITE)
		popSquareOf = popFirstSquareOf;
	else if (color == BLACK)
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
				move.setPawnPromotion();
				setCapture(bitBoard, move, color, to);

				for (Piece p = QUEEN; p > PAWN; --p) {
					move.setPromotionPiece(p);
					moves[movesCount] = move;
					moves[movesCount++].score = PIECE_VALUE[p];
				}
				continue;
			}
			if (color == WHITE && to - from == 16 ||
				color == BLACK && from - to == 16)
				move.setPawnStart();
			else if (to == bitBoard.getEnPassantSquare())
				move.setEnPassantCapture();
			else
				setCapture(bitBoard, move, color, to);

			moves[movesCount++] = move;
		}
	} else {
		if (piece == KING)
			if (!attacks::isSquareAttacked(bitBoard, ~color, getFirstSquareOf(bitBoard.getBitmapPiece(KING, color)))) {//King is not in check
				if (canMakeKingCastle(bitBoard, color)) {
					move.setFrom(from);
					move.setTo(from + 2U);
					move.setKingCastle();
					move.setColor(color);
					moves[movesCount++] = move;
				}
				if (canMakeQueenCastle(bitBoard, color)) {
					move.setFrom(from);
					move.setTo(from - 2U);
					move.setQueenCastle();
					move.setColor(color);
					moves[movesCount++] = move;
				}
			}
		while (attacks) {
			to = popSquareOf(attacks);

			move();
			move.setFrom(from);
			move.setTo(to);
			move.setColor(color);
			setCapture(bitBoard, move, color, to);

			moves[movesCount++] = move;
		}
	}
}

uShort moveGenerator::generateMoves(const BitBoard& bitBoard, Move moves[]) {
	const Color color = bitBoard.getColorTime();
	Bitmap attacks = 0UL;
	Bitmap pieceBitmap;
	Square square = NONE_SQUARE;
	const Square(*popSquareOf)(Bitmap&) = nullptr;

	if (color == WHITE)
		popSquareOf = popFirstSquareOf;
	else if (color == BLACK)
		popSquareOf = popLastSquareOf;

	friendPieces = bitBoard.getBitmapAllPieces(color);
	enemyPieces = bitBoard.getBitmapAllPieces(~color);
	movesCount = 0U;

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