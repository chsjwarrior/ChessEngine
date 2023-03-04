#include "MoveGenerator.h"

static BitBoard friendPieces;
static BitBoard enemyPieces;
static uShort movesCount;

/* This function tests if is possible to make King castle */
static bool canMakeKingCastle(const Board& board, const Color color) {
	if (color == WHITE && board.hasCastlePermission(WHITE_KING_CASTLE) ||
		color == BLACK && board.hasCastlePermission(BLACK_KING_CASTLE))
		if (((board.getBitBoardOfPiece(KING, color) << 1U | board.getBitBoardOfPiece(KING, color) << 2U) & (friendPieces | enemyPieces)) == 0) { //isCastlePathClear
			const Rank relativeRank = getRelativeRank(color, RANK_1);
			if (!attacks::isSquareAttacked(board, ~color, getSquareOf(FILE_F, relativeRank)))//isCastlePathSecury
				return !attacks::isSquareAttacked(board, ~color, getSquareOf(FILE_G, relativeRank));
		}
	return false;
}

/* This function tests if is possible to make Queen castle */
static bool canMakeQueenCastle(const Board& board, const Color color) {
	if (color == WHITE && board.hasCastlePermission(WHITE_QUEEN_CASTLE) ||
		color == BLACK && board.hasCastlePermission(BLACK_QUEEN_CASTLE))
		if (((board.getBitBoardOfPiece(KING, color) >> 1U | board.getBitBoardOfPiece(KING, color) >> 2U | board.getBitBoardOfPiece(KING, color) >> 3U) & (friendPieces | enemyPieces)) == 0) {//isCastlePathClear
			const Rank relativeRank = getRelativeRank(color, RANK_1);
			if (!attacks::isSquareAttacked(board, ~color, getSquareOf(FILE_C, relativeRank)))//isCastlePathSecury
				return !attacks::isSquareAttacked(board, ~color, getSquareOf(FILE_D, relativeRank));//isCastlePathSecury
		}
	return false;
}

enum MoveType { QUIETS, CAPTURES, ALL };
/* This function returns a bitmap with the attacked squares */
template<MoveType moveType>
static BitBoard getPieceAttacks(const Board& board, const Piece piece, const Color color, const Square square) {
	BitBoard attacks = 0UL;
	if (piece == KING)
		attacks = attacks::getKingAttacks(SQUARE_MASK[square]);
	else if (piece == KNIGHT)
		attacks = attacks::getKnightAttacks(SQUARE_MASK[square]);
	else {
		const BitBoard allPieces = friendPieces | enemyPieces;

		if (piece == PAWN) {
			if (moveType == ALL) {
				attacks = attacks::getPawnMoves(allPieces, color, SQUARE_MASK[square]);
				attacks |= attacks::getPawnAttacks(enemyPieces, color, SQUARE_MASK[square]);
				attacks |= attacks::getPawnEnPassantAttack(color, SQUARE_MASK[board.getEnPassantSquare()], SQUARE_MASK[square]);
			} else if (moveType == CAPTURES) {
				attacks = attacks::getPawnAttacks(enemyPieces, color, SQUARE_MASK[square]);
				attacks |= attacks::getPawnEnPassantAttack(color, SQUARE_MASK[board.getEnPassantSquare()], SQUARE_MASK[square]);
			} else if (moveType == QUIETS)
				attacks = attacks::getPawnMoves(allPieces, color, SQUARE_MASK[square]);
			return attacks;
		} else {
			const File file = getFileOf(square);
			const Rank rank = getRankOf(square);

			if (piece == BISHOP)
				attacks = attacks::getBishopAttacks(allPieces, file, rank, SQUARE_MASK[square]);
			else if (piece == ROOK)
				attacks = attacks::getRookAttacks(allPieces, file, rank, SQUARE_MASK[square]);
			else if (piece == QUEEN)
				attacks = attacks::getQueenAttacks(allPieces, file, rank, SQUARE_MASK[square]);
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
static void catalogCastleMoves(const Board& board, Move moves[], const Color color) {
	Square kingSquare = getFirstSquareOf(board.getBitBoardOfPiece(KING, color));
	if (!attacks::isSquareAttacked(board, ~color, kingSquare)) {//if King is not in check
		Move move;
		if (canMakeKingCastle(board, color)) {
			move.setFrom(kingSquare);
			move.setTo(kingSquare + 2U);
			move.setColor(color);
			move.setCastle();
			moves[movesCount++] = move;
		}
		if (canMakeQueenCastle(board, color)) {
			move.setFrom(kingSquare);
			move.setTo(kingSquare - 2U);
			move.setColor(color);
			move.setCastle();
			moves[movesCount++] = move;
		}
	}
}

/* This function populates an array with the moves */
static void catalogMoves(const Board& board, Move moves[], const Piece piece, const Color color, const Square from, BitBoard attacks) {
	Square to;
	Move move;
	Square(*popSquareOf)(BitBoard&);
	if (color == WHITE)
		popSquareOf = popFirstSquareOf;
	else
		popSquareOf = popLastSquareOf;

	if (piece == PAWN) {
		while (attacks) {
			to = popSquareOf(attacks);

			move.setFrom(from);
			move.setTo(to);
			move.setColor(color);
			move.setCaptured(board.getPieceFromSquare(~color, to));

			if (color == WHITE && getRankOf(to) == RANK_8 ||
				color == BLACK && getRankOf(to) == RANK_1) {
				for (Piece p = QUEEN; p > PAWN; --p) {
					move.setPromotionPiece(p);
					moves[movesCount++] = move;
				}
				continue;
			}
			if (!move.isCapture())
				if (color == WHITE && move.getTo() - move.getFrom() == 16 ||
					color == BLACK && move.getFrom() - move.getTo() == 16)
					move.setPawnStart();
				else if (to == board.getEnPassantSquare())
					move.setEnPassantCapture();

			moves[movesCount++] = move;
			move();
		}
	} else if (piece == KING) {
		while (attacks) {
			to = popSquareOf(attacks);

			if (attacks::isSquareAttacked(board, ~color, to))
				continue;

			move.setFrom(from);
			move.setTo(to);
			move.setColor(color);
			move.setCaptured(board.getPieceFromSquare(~color, to));

			moves[movesCount++] = move;
			move();
		}
	} else {
		while (attacks) {
			to = popSquareOf(attacks);

			move.setFrom(from);
			move.setTo(to);
			move.setColor(color);
			move.setCaptured(board.getPieceFromSquare(~color, to));

			moves[movesCount++] = move;
			move();
		}
	}
}

/* This function generates the moves with the move type you need */
template<MoveType moveType>
static uShort generateMoves(const Board& board, Move moves[]) {
	const Color color = board.getColorTime();
	friendPieces = board.getBitBoardOfAllPieces(color);
	enemyPieces = board.getBitBoardOfAllPieces(~color);
	movesCount = 0U;

	BitBoard attacks;
	BitBoard pieceBitBoard;
	Square square;
	Square(*popSquareOf)(BitBoard&);
	if (color == WHITE)
		popSquareOf = popFirstSquareOf;
	else
		popSquareOf = popLastSquareOf;

	for (Piece p = PAWN; p != NONE_PIECE; ++p) {
		pieceBitBoard = board.getBitBoardOfPiece(p, color);
		while (pieceBitBoard) {
			square = popSquareOf(pieceBitBoard);
			attacks = getPieceAttacks<moveType>(board, p, color, square);
			catalogMoves(board, moves, p, color, square, attacks);
		}
	}
	if (moveType != CAPTURES)
		catalogCastleMoves(board, moves, color);
	return movesCount;
}

uShort moveGenerator::generateAllMoves(const Board& board, Move moves[], const Square square) {
	const Color color = board.getColorTime();
	const Piece piece = board.getPieceFromSquare(color, square);

	if (piece == NONE_PIECE)
		return 0U;

	friendPieces = board.getBitBoardOfAllPieces(color);
	enemyPieces = board.getBitBoardOfAllPieces(~color);
	movesCount = 0U;

	BitBoard attacks = getPieceAttacks<ALL>(board, piece, color, square);
	catalogMoves(board, moves, piece, color, square, attacks);
	if (piece == KING)
		catalogCastleMoves(board, moves, color);
	return movesCount;
}

uShort moveGenerator::generateAllMoves(const Board& board, Move moves[]) {
	return generateMoves<ALL>(board, moves);
}

uShort generateQuietMoves(const Board& board, Move moves[]) {
	return generateMoves<QUIETS>(board, moves);
}

uShort moveGenerator::generateCaptureMoves(const Board& board, Move moves[]) {
	return generateMoves<CAPTURES>(board, moves);
}

bool moveGenerator::moveExists(Board& board, const Move move) {
	Move moves[MAX_MOVES];
	generateAllMoves(board, moves, move.getFrom());

	uShort i = 0U;
	while (i < movesCount) {
		if (move == moves[i])//if found
			if (makeMove(board, moves[i])) {//if is valid move
				makeUndo(board);
				break;
			}
		++i;
	}

	return i < movesCount;
}