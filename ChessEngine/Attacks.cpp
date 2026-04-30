#include "Attacks.h"

BitBoard attacks::getQueenAttacks(const BitBoard occupieds, const Square square) {
	return getBishopAttacks(occupieds, square) | getRookAttacks(occupieds, square);
}

static inline BitBoard hyperbolaQuintessence(const BitBoard occupieds, const BitBoard mask, const BitBoard square) {
	BitBoard forward = occupieds & mask;
	BitBoard reverse = getReverse(forward);
	forward -= (square << 1);// 2 * square
	reverse -= (getReverse(square) << 1);// 2 * reverse square
	reverse = getReverse(reverse);
	return (forward ^ reverse) & mask;
}

BitBoard attacks::getBishopAttacks(const BitBoard occupieds, const Square square) {
	const BitBoard dMoves = hyperbolaQuintessence(occupieds, DIAGONAL[getDiagonalOf(getFileOf(square), getRankOf(square))], SQUARE_MASK[square]);
	const BitBoard adMoves = hyperbolaQuintessence(occupieds, ANTI_DIAGONAL[getAntiDiagonalOf(getFileOf(square), getRankOf(square))], SQUARE_MASK[square]);
	return dMoves | adMoves;
}

BitBoard attacks::getRookAttacks(const BitBoard occupieds, const Square square) {
	BitBoard forward = occupieds - (SQUARE_MASK[square] << 1);// 2 * square
	BitBoard reverse = getReverse(occupieds);
	reverse -= (getReverse(SQUARE_MASK[square]) << 1);// 2 * reverse square
	reverse = getReverse(reverse);
	const BitBoard hMoves = (forward ^ reverse) & RANKS[getRankOf(square)];
	const BitBoard vMoves = hyperbolaQuintessence(occupieds, FILES[getFileOf(square)], SQUARE_MASK[square]);
	return hMoves | vMoves;
}

BitBoard attacks::getKnightAttacks(const Square square) {
	BitBoard moves = SQUARE_MASK[square] << 10 & ~(FILES[FILE_A] | FILES[FILE_B]);
	moves |= SQUARE_MASK[square] << 17 & ~FILES[FILE_A];
	moves |= SQUARE_MASK[square] << 6 & ~(FILES[FILE_G] | FILES[FILE_H]);
	moves |= SQUARE_MASK[square] << 15 & ~FILES[FILE_H];

	moves |= SQUARE_MASK[square] >> 10 & ~(FILES[FILE_G] | FILES[FILE_H]);
	moves |= SQUARE_MASK[square] >> 17 & ~FILES[FILE_H];
	moves |= SQUARE_MASK[square] >> 6 & ~(FILES[FILE_A] | FILES[FILE_B]);
	moves |= SQUARE_MASK[square] >> 15 & ~FILES[FILE_A];
	return moves;
}

BitBoard attacks::getKingAttacks(const Square square) {
	BitBoard moves = SQUARE_MASK[square] << 8 | SQUARE_MASK[square] >> 8;
	moves |= SQUARE_MASK[square] << 1 & ~FILES[FILE_A];
	moves |= SQUARE_MASK[square] << 9 & ~FILES[FILE_A];
	moves |= SQUARE_MASK[square] << 7 & ~FILES[FILE_H];

	moves |= SQUARE_MASK[square] >> 1 & ~FILES[FILE_H];
	moves |= SQUARE_MASK[square] >> 9 & ~FILES[FILE_H];
	moves |= SQUARE_MASK[square] >> 7 & ~FILES[FILE_A];
	return moves;
}

BitBoard attacks::getPawnMoves(const BitBoard occupieds, const Color color, const Square square) {
	BitBoard moves = 0ULL;
	if (color == WHITE) {
		moves |= SQUARE_MASK[square] << 8 & ~occupieds;//normal move		
		moves |= SQUARE_MASK[square] << 16 & ~occupieds & ~occupieds << 8 & RANKS[RANK_4];//start move
	}
	else {
		moves |= SQUARE_MASK[square] >> 8 & ~occupieds;//normal move		
		moves |= SQUARE_MASK[square] >> 16 & ~occupieds & ~occupieds >> 8 & RANKS[RANK_5];//start move
	}
	return moves;
}

BitBoard attacks::getPawnAttacks(const Color color, const Square square) {
	BitBoard moves = 0ULL;
	if (color == WHITE) {
		moves |= SQUARE_MASK[square] << 7 & ~FILES[FILE_H];
		moves |= SQUARE_MASK[square] << 9 & ~FILES[FILE_A];
	}
	else {
		moves |= SQUARE_MASK[square] >> 7 & ~FILES[FILE_A];
		moves |= SQUARE_MASK[square] >> 9 & ~FILES[FILE_H];
	}
	return moves;
}

BitBoard attacks::getPawnAttacks(const BitBoard occupieds, const Color color, const Square square) {
	return getPawnAttacks(color, square) & occupieds;// Pawn moves to diagonal only if the square is occupied
}

BitBoard attacks::getPawnEnPassantAttack(const Color color, const Square enPassant, const Square square) {
	// relative rank: 6 for White and 3 for Black
	return getPawnAttacks(color, square) & SQUARE_MASK[enPassant] & RANKS[getRelativeRankOf(color, RANK_6)];
}

bool attacks::isSquareAttacked(const Board& board, const Color color, const Square square) {
	// King
	BitBoard attacks = getKingAttacks(square);
	if (attacks & board.getBitBoardOf(KING, color))
		return true;
	// Knight
	attacks = getKnightAttacks(square);
	if (attacks & board.getBitBoardOf(KNIGHT, color))
		return true;
	// Pawn
	attacks = getPawnAttacks(~color, square);
	if (attacks & board.getBitBoardOf(PAWN, color))
		return true;

	const BitBoard occupieds = board.getBitBoardOf(WHITE) | board.getBitBoardOf(BLACK);

	// Bishop
	attacks = getBishopAttacks(occupieds, square);
	if (attacks & board.getBitBoardOf(BISHOP, color) || attacks & board.getBitBoardOf(QUEEN, color))
		return true;
	// Rook
	attacks = getRookAttacks(occupieds, square);
	if (attacks & board.getBitBoardOf(ROOK, color) || attacks & board.getBitBoardOf(QUEEN, color))
		return true;
	return false;
}