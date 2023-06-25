#include "MoveGenerator.h"

static BitBoard friendBitBoard;
static BitBoard enemyBitBoard;
static uShort movesCount;

/* This function tests if is possible to make King castle */
static bool canMakeKingCastle(const Board& board, const Color color) {
	if (color == WHITE && board.hasCastlePermission(WHITE_KING_CASTLE) ||
		color == BLACK && board.hasCastlePermission(BLACK_KING_CASTLE)) {
		const Rank relativeRank = getRelativeRankOf(color, RANK_1);
		const Square f = getSquareOf(FILE_F, relativeRank);
		const Square g = getSquareOf(FILE_G, relativeRank);
		if (((SQUARE_MASK[f] | SQUARE_MASK[g]) & (friendBitBoard | enemyBitBoard)) == 0)  // isCastlePathClear
			if (!attacks::isSquareAttacked(board, ~color, f))// isCastlePathSecury
				return !attacks::isSquareAttacked(board, ~color, g);// isCastlePathSecury
	}
	return false;
}

/* This function tests if is possible to make Queen castle */
static bool canMakeQueenCastle(const Board& board, const Color color) {
	if (color == WHITE && board.hasCastlePermission(WHITE_QUEEN_CASTLE) ||
		color == BLACK && board.hasCastlePermission(BLACK_QUEEN_CASTLE)) {
		const Rank relativeRank = getRelativeRankOf(color, RANK_1);
		const Square c = getSquareOf(FILE_C, relativeRank);
		const Square d = getSquareOf(FILE_D, relativeRank);
		if (((SQUARE_MASK[c] | SQUARE_MASK[d] | SQUARE_MASK[getSquareOf(FILE_B, relativeRank)]) & (friendBitBoard | enemyBitBoard)) == 0) // isCastlePathClear
			if (!attacks::isSquareAttacked(board, ~color, c))// isCastlePathSecury
				return !attacks::isSquareAttacked(board, ~color, d);// isCastlePathSecury
	}
	return false;
}

enum MoveType { ALL, CAPTURES, QUIETS };// Quiet moves has not material change
/* This function returns a bitmap with the attacked squares */
static BitBoard getPieceAttacks(const Board& board, const Piece piece, const Color color, const Square square, const MoveType moveType) {
	BitBoard attacks = 0ULL;
	if (piece == KING)
		attacks = attacks::getKingAttacks(SQUARE_MASK[square]);
	else if (piece == KNIGHT)
		attacks = attacks::getKnightAttacks(SQUARE_MASK[square]);
	else {
		const BitBoard occupieds = friendBitBoard | enemyBitBoard;

		if (piece == PAWN) {
			if (moveType == ALL) {
				attacks = attacks::getPawnMoves(occupieds, color, SQUARE_MASK[square]);
				attacks |= attacks::getPawnAttacks(enemyBitBoard, color, SQUARE_MASK[square]);
				attacks |= attacks::getPawnEnPassantAttack(color, SQUARE_MASK[board.getEnPassantSquare()], SQUARE_MASK[square]);
			} else if (moveType == CAPTURES) {
				attacks = attacks::getPawnAttacks(enemyBitBoard, color, SQUARE_MASK[square]);
				attacks |= attacks::getPawnEnPassantAttack(color, SQUARE_MASK[board.getEnPassantSquare()], SQUARE_MASK[square]);
			} else if (moveType == QUIETS)// Quiets move can not have pawn promotion
				attacks = attacks::getPawnMoves(occupieds, color, SQUARE_MASK[square]) & ~RANKS[getRelativeRankOf(color, RANK_8)];
			return attacks;
		} else {
			const File file = getFileOf(square);
			const Rank rank = getRankOf(square);

			if (piece == BISHOP)
				attacks = attacks::getBishopAttacks(occupieds, file, rank, SQUARE_MASK[square]);
			else if (piece == ROOK)
				attacks = attacks::getRookAttacks(occupieds, file, rank, SQUARE_MASK[square]);
			else if (piece == QUEEN)
				attacks = attacks::getQueenAttacks(occupieds, file, rank, SQUARE_MASK[square]);
		}
	}
	attacks &= ~friendBitBoard;// remove friend bitmap
	if (moveType == QUIETS)
		attacks &= ~enemyBitBoard;// remove enemy bitmap
	else if (moveType == CAPTURES)
		attacks &= enemyBitBoard;// keep enemy bitmap
	return attacks;
}

/* This function populates an array with the castle moves */
static void catalogCastleMoves(const Board& board, Move moves[], const Color color) {
	Square kingSquare = getFirstSquareOf(board.getBitBoardOf(KING, color));
	if (!attacks::isSquareAttacked(board, ~color, kingSquare)) {// if King is not in check
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
	Square(*popSquareOf)(BitBoard&) = color == WHITE ? popFirstSquareOf : popLastSquareOf;

	if (piece == PAWN) {
		while (attacks) {
			to = popSquareOf(attacks);

			move.setFrom(from);
			move.setTo(to);
			move.setColor(color);
			move.setCaptured(board.getPieceFromSquare(~color, to));

			// if (getRelativeRankOf(color, RANK_8) == getRankOf(to))
			if (RANKS[getRelativeRankOf(color, RANK_8)] & SQUARE_MASK[to]) {
				for (Piece p = QUEEN; p > PAWN; --p) {
					move.setPromotionPiece(p);
					moves[movesCount++] = move;
				}
				continue;
			}
			if (!move.isCapture())
				if (std::abs(move.getTo() - move.getFrom()) == 16)
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
static uShort generateMoves(const Board& board, Move moves[], const MoveType moveType) {
	const Color color = board.getColorTime();
	friendBitBoard = board.getBitBoardOf(color);
	enemyBitBoard = board.getBitBoardOf(~color);
	movesCount = 0U;

	BitBoard attacks;
	BitBoard pieceBitBoard;
	Square square;
	Square(*popSquareOf)(BitBoard&) = color == WHITE ? popFirstSquareOf : popLastSquareOf;

	for (Piece p = PAWN; p != NONE_PIECE; ++p) {
		pieceBitBoard = board.getBitBoardOf(p, color);
		while (pieceBitBoard) {
			square = popSquareOf(pieceBitBoard);
			attacks = getPieceAttacks(board, p, color, square, moveType);
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

	friendBitBoard = board.getBitBoardOf(color);
	enemyBitBoard = board.getBitBoardOf(~color);
	movesCount = 0U;

	BitBoard attacks = getPieceAttacks(board, piece, color, square, ALL);
	catalogMoves(board, moves, piece, color, square, attacks);
	if (piece == KING)
		catalogCastleMoves(board, moves, color);
	return movesCount;
}

uShort moveGenerator::generateAllMoves(const Board& board, Move moves[]) {
	return generateMoves(board, moves, ALL);
}

uShort moveGenerator::generateQuietMoves(const Board& board, Move moves[]) {
	return generateMoves(board, moves, QUIETS);
}

uShort moveGenerator::generateCaptureMoves(const Board& board, Move moves[]) {
	return generateMoves(board, moves, CAPTURES);
}

bool moveGenerator::moveExists(Board& board, const Move move) {
	Move moves[MAX_MOVES];
	generateAllMoves(board, moves, move.getFrom());

	uShort i = 0U;
	while (i < movesCount) {
		if (move == moves[i])// if found
			if (makeMove(board, moves[i])) {// if is valid move
				makeUndo(board);
				break;
			}
		++i;
	}

	return i < movesCount;
}