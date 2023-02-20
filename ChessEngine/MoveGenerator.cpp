#include "MoveGenerator.h"

static Bitmap friendPieces;
static Bitmap enemyPieces;
static uShort movesCount;

enum MoveType { QUIETS, CAPTURES, ALL };

/* This function tests if is possible to make King castle */
static bool canMakeKingCastle(const BitBoard& bitBoard, const Color color) {
	if (color == WHITE && bitBoard.hasCastlePermission(WHITE_KING_CASTLE) ||
		color == BLACK && bitBoard.hasCastlePermission(BLACK_KING_CASTLE))
		if (((bitBoard.getBitmapPiece(KING, color) << 1U | bitBoard.getBitmapPiece(KING, color) << 2U) & (friendPieces | enemyPieces)) == 0) { //isCastlePathClear
			const Rank relativeRank = color == WHITE ? RANK_1 : RANK_8;
			if (!attacks::isSquareAttacked(bitBoard, ~color, getSquareOf(FILE_F, relativeRank)))//isCastlePathSecury
				return !attacks::isSquareAttacked(bitBoard, ~color, getSquareOf(FILE_G, relativeRank));
		}
	return false;
}

/* This function tests if is possible to make Queen castle */
static bool canMakeQueenCastle(const BitBoard& bitBoard, const Color color) {
	if (color == WHITE && bitBoard.hasCastlePermission(WHITE_QUEEN_CASTLE) ||
		color == BLACK && bitBoard.hasCastlePermission(BLACK_QUEEN_CASTLE))
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

/* This function returns a bitmap with the attacked squares */
template<MoveType moveType>
static Bitmap getPiecesMoves(const BitBoard& bitBoard, const Piece piece, const Color color, const Square square) {
	Bitmap attacks = 0UL;
	if (piece == KING)
		attacks = attacks::getKingMoves(getBitmapOf(square));
	else if (piece == KNIGHT)
		attacks = attacks::getKnightMoves(getBitmapOf(square));
	else {
		const Bitmap allPieces = friendPieces | enemyPieces;

		if (piece == PAWN) {
			attacks = attacks::getPawnMoves(allPieces, color, getBitmapOf(bitBoard.getEnPassantSquare()), getBitmapOf(square));
			if (moveType == QUIETS)
				return attacks & ~enemyPieces | getBitmapOf(bitBoard.getEnPassantSquare());
			else if (moveType == CAPTURES)
				return attacks & enemyPieces | getBitmapOf(bitBoard.getEnPassantSquare());
		} else {
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
	attacks &= ~friendPieces;//remove friend bitmap
	if (moveType == QUIETS)
		attacks &= ~enemyPieces;//remove enemy bitmap
	else if (moveType == CAPTURES)
		attacks &= enemyPieces;//keep enemy bitmap
	return attacks;
}

/* This function populates an array with the castle moves */
static void catalogCastleMoves(const BitBoard& bitBoard, Move moves[], const Color color) {
	Square kingSquare = getFirstSquareOf(bitBoard.getBitmapPiece(KING, color));
	if (!attacks::isSquareAttacked(bitBoard, ~color, kingSquare)) {//if King is not in check
		Move move;
		if (canMakeKingCastle(bitBoard, color)) {
			move.setFrom(kingSquare);
			move.setTo(kingSquare + 2U);
			move.setColor(color);
			move.setCastle();
			moves[movesCount++] = move;
		}
		if (canMakeQueenCastle(bitBoard, color)) {
			move.setFrom(kingSquare);
			move.setTo(kingSquare - 2U);
			move.setColor(color);
			move.setCastle();
			moves[movesCount++] = move;
		}
	}
}

/* This function populates an array with the moves */
static void catalogMoves(const BitBoard& bitBoard, Move moves[], const Piece piece, const Color color, const Square from, Bitmap attacks) {
	Square to;
	Move move;
	Square(*popSquareOf)(Bitmap&) = popFirstSquareOf;
	if (color == BLACK)
		popSquareOf = popLastSquareOf;

	if (piece == PAWN) {
		while (attacks) {
			to = popSquareOf(attacks);

			move.setFrom(from);
			move.setTo(to);
			move.setColor(color);
			setCapture(bitBoard, move, color, to);

			if (color == WHITE && getRankOf(to) == RANK_8 ||
				color == BLACK && getRankOf(to) == RANK_1) {
				for (Piece p = QUEEN; p > PAWN; --p) {
					move.setPromotionPiece(p);
					moves[movesCount] = move;
					moves[movesCount++].score += PIECE_VALUE[p];
				}
				continue;
			}
			if (color == WHITE && move.getTo() - move.getFrom() == 16 ||
				color == BLACK && move.getFrom() - move.getTo() == 16)
				move.setPawnStart();
			else if (to == bitBoard.getEnPassantSquare())
				move.setEnPassantCapture();

			moves[movesCount++] = move;
			move();
		}
	} else {
		while (attacks) {
			to = popSquareOf(attacks);

			move.setFrom(from);
			move.setTo(to);
			move.setColor(color);
			setCapture(bitBoard, move, color, to);

			moves[movesCount++] = move;
			move();
		}
	}
}

/* This function generates the moves with the move type you need */
template<MoveType moveType>
static uShort generateMoves(const BitBoard& bitBoard, Move moves[]) {
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
			attacks = getPiecesMoves<moveType>(bitBoard, p, color, square);
			catalogMoves(bitBoard, moves, p, color, square, attacks);
		}
	}
	if (moveType != CAPTURES)
		catalogCastleMoves(bitBoard, moves, color);
	return movesCount;
}

uShort moveGenerator::generateAllMoves(const BitBoard& bitBoard, Move moves[], const Square square) {
	const Color color = bitBoard.getColorTime();
	const Piece piece = bitBoard.getPieceFromSquare(color, square);

	if (piece == NONE_PIECE)
		return 0U;

	friendPieces = bitBoard.getBitmapAllPieces(color);
	enemyPieces = bitBoard.getBitmapAllPieces(~color);
	movesCount = 0U;

	Bitmap attacks = getPiecesMoves<ALL>(bitBoard, piece, color, square);
	catalogMoves(bitBoard, moves, piece, color, square, attacks);
	if (piece == KING)
		catalogCastleMoves(bitBoard, moves, color);
	return movesCount;
}

uShort moveGenerator::generateAllMoves(const BitBoard& bitBoard, Move moves[]) {
	return generateMoves<ALL>(bitBoard, moves);
}

uShort generateQuietMoves(const BitBoard& bitBoard, Move moves[]) {
	return generateMoves<QUIETS>(bitBoard, moves);
}

uShort moveGenerator::generateCaptureMoves(const BitBoard& bitBoard, Move moves[]) {
	return generateMoves<CAPTURES>(bitBoard, moves);
}

bool moveGenerator::moveExists(BitBoard& bitBoard, const Move move) {
	Move moves[MAX_MOVES];
	generateAllMoves(bitBoard, moves, move.getFrom());

	uShort i = 0U;
	while (i < movesCount) {
		if (move == moves[i])//if found
			if (makeMove(bitBoard, moves[i])) {//if is valid move
				makeUndo(bitBoard);
				break;
			}
		++i;
	}

	return i < movesCount;
}