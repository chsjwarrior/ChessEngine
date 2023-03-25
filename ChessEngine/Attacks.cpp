#include "Attacks.h"

BitBoard attacks::getQueenAttacks(const BitBoard occupieds, const File file, const Rank rank, const BitBoard square) {
	return getBishopAttacks(occupieds, file, rank, square) | getRookAttacks(occupieds, file, rank, square);
}

static inline BitBoard hyperbolaQuintessence(const BitBoard occupieds, const BitBoard mask, const BitBoard square) {
	BitBoard forward = occupieds & mask;
	BitBoard reverse = getReverse(forward);
	forward -= (square << 1);// 2 * square
	reverse -= (getReverse(square) << 1);// 2 * reverse square
	reverse = getReverse(reverse);
	return (forward ^ reverse) & mask;
}

BitBoard attacks::getBishopAttacks(const BitBoard occupieds, const File file, const Rank rank, const BitBoard square) {
	const BitBoard dMoves = hyperbolaQuintessence(occupieds, DIAGONAL[getDiagonalOf(file, rank)], square);
	const BitBoard adMoves = hyperbolaQuintessence(occupieds, ANTI_DIAGONAL[getAntiDiagonalOf(file, rank)], square);
	return dMoves | adMoves;
}

BitBoard attacks::getRookAttacks(const BitBoard occupieds, const File file, const Rank rank, const BitBoard square) {
	BitBoard forward = occupieds - (square << 1);// 2 * square
	BitBoard reverse = getReverse(occupieds);
	reverse -= (getReverse(square) << 1);// 2 * reverse square
	reverse = getReverse(reverse);
	const BitBoard hMoves = (forward ^ reverse) & RANKS[rank];
	const BitBoard vMoves = hyperbolaQuintessence(occupieds, FILES[file], square);
	return hMoves | vMoves;
}

BitBoard attacks::getKnightAttacks(const BitBoard square) {
	BitBoard moves = square << 10 & ~(FILES[FILE_A] | FILES[FILE_B]);
	moves |= square << 17 & ~FILES[FILE_A];
	moves |= square << 6 & ~(FILES[FILE_G] | FILES[FILE_H]);
	moves |= square << 15 & ~FILES[FILE_H];

	moves |= square >> 10 & ~(FILES[FILE_G] | FILES[FILE_H]);
	moves |= square >> 17 & ~FILES[FILE_H];
	moves |= square >> 6 & ~(FILES[FILE_A] | FILES[FILE_B]);
	moves |= square >> 15 & ~FILES[FILE_A];
	return moves;
}

BitBoard attacks::getKingAttacks(const BitBoard square) {
	BitBoard moves = square << 8 | square >> 8;
	moves |= square << 1 & ~FILES[FILE_A];
	moves |= square << 9 & ~FILES[FILE_A];
	moves |= square << 7 & ~FILES[FILE_H];

	moves |= square >> 1 & ~FILES[FILE_H];
	moves |= square >> 9 & ~FILES[FILE_H];
	moves |= square >> 7 & ~FILES[FILE_A];
	return moves;
}

BitBoard attacks::getPawnMoves(const BitBoard occupieds, const Color color, const BitBoard square) {
	BitBoard moves = 0UL;
	if (color == WHITE) {
		moves |= square << 8 & ~occupieds;//normal move		
		moves |= square << 16 & ~occupieds & ~occupieds << 8 & RANKS[RANK_4];//start move
	} else {
		moves |= square >> 8 & ~occupieds;//normal move		
		moves |= square >> 16 & ~occupieds & ~occupieds >> 8 & RANKS[RANK_5];//start move
	}
	return moves;
}

BitBoard attacks::getPawnAttacks(const Color color, const BitBoard square) {
	BitBoard moves = 0UL;
	if (color == WHITE) {
		moves |= square << 7 & ~FILES[FILE_H];
		moves |= square << 9 & ~FILES[FILE_A];
	} else {
		moves |= square >> 7 & ~FILES[FILE_A];
		moves |= square >> 9 & ~FILES[FILE_H];
	}
	return moves;
}

BitBoard attacks::getPawnAttacks(const BitBoard occupieds, const Color color, const BitBoard square) {
	return getPawnAttacks(color, square) & occupieds;// Pawn moves to diagonal only if the square is occupied
}

BitBoard attacks::getPawnEnPassantAttack(const Color color, const BitBoard enPassant, const BitBoard square) {
	// relative rank: 6 for White and 3 for Black
	return getPawnAttacks(color, square) & enPassant & RANKS[getRelativeRankOf(color, RANK_6)];
}

bool attacks::isSquareAttacked(const Board& board, const Color color, const Square square) {
	// King
	BitBoard attacks = getKingAttacks(SQUARE_MASK[square]);
	if (attacks & board.getBitBoardOf(KING, color))
		return true;
	// Knight
	attacks = getKnightAttacks(SQUARE_MASK[square]);
	if (attacks & board.getBitBoardOf(KNIGHT, color))
		return true;
	// Pawn
	attacks = getPawnAttacks(~color, SQUARE_MASK[square]);
	if (attacks & board.getBitBoardOf(PAWN, color))
		return true;

	const File file = getFileOf(square);
	const Rank rank = getRankOf(square);
	const BitBoard occupieds = board.getBitBoardOf(WHITE) | board.getBitBoardOf(BLACK);

	// Bishop
	attacks = getBishopAttacks(occupieds, file, rank, SQUARE_MASK[square]);
	if (attacks & board.getBitBoardOf(BISHOP, color) || attacks & board.getBitBoardOf(QUEEN, color))
		return true;
	// Rook
	attacks = getRookAttacks(occupieds, file, rank, SQUARE_MASK[square]);
	if (attacks & board.getBitBoardOf(ROOK, color) || attacks & board.getBitBoardOf(QUEEN, color))
		return true;
	return false;
}