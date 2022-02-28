#include "MoveGenerator.h"

MoveGenerator& MoveGenerator::getInstance() {
	static MoveGenerator instance;
	return instance;
}

void MoveGenerator::setCapture(const BitBoard& bitBoard, Move& move, const Color color, const Square to) const {
	const Piece capture = bitBoard.getPieceFromSquare(~color, to);
	move.setCaptured(capture);
	move.score += PIECE_VALUE[capture];
}

const bool MoveGenerator::canMakeKingCastle(const BitBoard& bitBoard, const Color color) const {
	if (bitBoard.hasCastlePermission(KING_CASTLE, color)) {
		Bitmap pathKingCastle = getIntersections(PATH_KING_CASTLE, color == WHITE ? RANKS[RANK_1] : RANKS[RANK_8]);
		if (!hasIntersection(pathKingCastle, getUnion(friendPieces, enemyPieces))) {//isPathKingCastleClear
			while (pathKingCastle)//isPathKingCastleSecury
				if (attacks::isSquareAttacked(bitBoard, ~color, popLastSquareOf(pathKingCastle)))
					return false;
			return true;
		}
	}
	return false;
}

const bool MoveGenerator::canMakeQueenCastle(const BitBoard& bitBoard, const Color color) const {
	if (bitBoard.hasCastlePermission(QUEEN_CASTLE, color)) {
		const Bitmap relativeRank = color == WHITE ? RANKS[RANK_1] : RANKS[RANK_8];
		if (!hasIntersection(getIntersections(PATH_QUEEN_CASTLE, relativeRank), getUnion(friendPieces, enemyPieces))) {//isPathQueenCastleClear
			Bitmap pathKingCastle = getIntersections(PATH_KING_CASTLE >> 3, relativeRank);
			while (pathKingCastle)//isPathSecury
				if (attacks::isSquareAttacked(bitBoard, ~color, popFirstSquareOf(pathKingCastle)))
					return false;
			return true;
		}
	}
	return false;
}

const Bitmap MoveGenerator::generatePiecesMoves(const BitBoard& bitBoard, const Piece piece, const Color color, const Square square) const {
	const Bitmap squareBitmap = getBitmapOf(square);//1UL << square;
	Bitmap attacks = 0UL;

	if (piece == KNIGHT)
		attacks = attacks::getKnightMoves(squareBitmap);
	else if (piece == KING)
		attacks = attacks::getKingMoves(squareBitmap);
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

void MoveGenerator::catalogMoves(const BitBoard& bitBoard, Move moves[], const Piece piece, const Color color, const Square from, Bitmap attacks) {
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
					moves[moveCount] = move;
					moves[moveCount++].score = PIECE_VALUE[p];
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

			moves[moveCount++] = move;
		}
	}
	else {
		if (piece == KING)
			if (!attacks::isSquareAttacked(bitBoard, ~color, getFirstSquareOf(bitBoard.getBitmapPiece(KING, color)))) {//King is not in check
				if (canMakeKingCastle(bitBoard, color)) {
					move.setFrom(from);
					move.setTo(from + 2U);
					move.setKingCastle();
					move.setColor(color);
					moves[moveCount++] = move;
				}
				if (canMakeQueenCastle(bitBoard, color)) {
					move.setFrom(from);
					move.setTo(from - 2U);
					move.setQueenCastle();
					move.setColor(color);
					moves[moveCount++] = move;
				}
			}
		while (attacks) {
			to = popSquareOf(attacks);

			move();
			move.setFrom(from);
			move.setTo(to);
			move.setColor(color);
			setCapture(bitBoard, move, color, to);

			moves[moveCount++] = move;
		}
	}
}

const uShort MoveGenerator::generateMoves(const BitBoard& bitBoard, Move moves[]) {
	const Color color = bitBoard.getColorTime();
	Bitmap attacks = 0UL;
	Bitmap pieceBitmap;
	Square square = NONE_SQUARE;
	const Square(*popSquareOf)(Bitmap&) = popFirstSquareOf;

	if (color == BLACK)
		popSquareOf = popLastSquareOf;

	moveCount = 0U;
	friendPieces = bitBoard.getBitmapAllPieces(color);
	enemyPieces = bitBoard.getBitmapAllPieces(~color);

	for (Piece p = PAWN; p != NONE_PIECE; ++p) {
		pieceBitmap = bitBoard.getBitmapPiece(p, color);
		while (pieceBitmap) {
			square = popSquareOf(pieceBitmap);
			attacks = generatePiecesMoves(bitBoard, p, color, square);
			catalogMoves(bitBoard, moves, p, color, square, attacks);
		}
	}
	return moveCount;
}

const uShort MoveGenerator::generateCaptureMoves(const BitBoard& bitBoard, Move moves[]) {
	uShort moveCount = generateMoves(bitBoard, moves);

	for (short i = 0; i < moveCount; ++i)
		if (moves[i].getCaptured() == NONE_PIECE && !moves[i].isEnPassantCapture()) {
			moves[i]();
			std::swap(moves[i--], moves[--moveCount]);
		}

	return moveCount;
}