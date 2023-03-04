#include "Attacks.h"

//files Bitmap
static const BitBoard FILES[8] = { 0x0101010101010101UL, 0x0202020202020202UL , 0x0404040404040404UL, 0x0808080808080808UL,
								0x1010101010101010UL, 0x2020202020202020UL, 0x4040404040404040UL, 0x8080808080808080UL };
//ranks Bitmap
static const BitBoard RANKS[8] = { 0x00000000000000FFUL, 0x000000000000FF00UL, 0x0000000000FF0000UL, 0x00000000FF000000UL,
								0x000000FF00000000UL, 0x0000FF0000000000UL, 0x00FF000000000000UL, 0xFF00000000000000UL };

static const BitBoard DIAGONAL[15] = { 0x0000000000000001UL, 0x0000000000000102UL, 0x0000000000010204UL, 0x0000000001020408UL,
									0x0000000102040810UL, 0x0000010204081020UL, 0x0001020408102040UL, 0x0102040810204080UL,
									0x0204081020408000UL, 0x0408102040800000UL, 0x0810204080000000UL, 0x1020408000000000UL,
									0x2040800000000000UL, 0x4080000000000000UL, 0x8000000000000000UL };
static const BitBoard ANTI_DIAGONAL[15] = { 0x0000000000000080UL, 0x0000000000008040UL, 0x0000000000804020UL, 0x0000000080402010UL,
									0x0000008040201008UL, 0x0000804020100804UL, 0x0080402010080402UL, 0x8040201008040201UL,
									0x4020100804020100UL, 0x2010080402010000UL, 0x1008040201000000UL, 0x0804020100000000UL,
									0x0402010000000000UL, 0x0201000000000000UL, 0x0100000000000000UL };

BitBoard attacks::getQueenAttacks(const BitBoard allPieces, const File file, const Rank rank, const BitBoard square) {
	return getBishopAttacks(allPieces, file, rank, square) | getRookAttacks(allPieces, file, rank, square);
}

BitBoard attacks::getBishopAttacks(const BitBoard allPieces, const File file, const Rank rank, const BitBoard square) {
	const BitBoard reverseSquare = reverse(square);

	uShort index = rank + file;

	BitBoard aux1 = (allPieces & DIAGONAL[index]) - 2 * square;
	BitBoard aux2 = reverse(allPieces & DIAGONAL[index]) - 2 * reverseSquare;
	const BitBoard ldMoves = (aux1 ^ reverse(aux2)) & DIAGONAL[index];

	index = rank + 7 - file;

	aux1 = (allPieces & ANTI_DIAGONAL[index]) - 2 * square;
	aux2 = reverse(allPieces & ANTI_DIAGONAL[index]) - 2 * reverseSquare;
	const BitBoard dlMoves = (aux1 ^ reverse(aux2)) & ANTI_DIAGONAL[index];

	return ldMoves | dlMoves;
}

BitBoard attacks::getRookAttacks(const BitBoard allPieces, const File file, const Rank rank, const BitBoard square) {
	const BitBoard reverseSquare = reverse(square);

	BitBoard aux1 = allPieces - 2 * square;
	BitBoard aux2 = reverse(allPieces) - 2 * reverseSquare;
	const BitBoard hMoves = (aux1 ^ reverse(aux2)) & RANKS[rank];

	aux1 = (allPieces & FILES[file]) - 2 * square;
	aux2 = reverse(allPieces & FILES[file]) - 2 * reverseSquare;
	const BitBoard vMoves = (aux1 ^ reverse(aux2)) & FILES[file];

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

BitBoard attacks::getPawnMoves(const BitBoard allPieces, const Color color, const BitBoard square) {
	BitBoard moves = 0UL;
	if (color == WHITE) {
		moves |= square << 8 & ~allPieces;//normal move		
		moves |= square << 16 & ~allPieces & ~allPieces << 8 & RANKS[RANK_4];//start move
	} else {
		moves |= square >> 8 & ~allPieces;//normal move		
		moves |= square >> 16 & ~allPieces & ~allPieces >> 8 & RANKS[RANK_5];//start move
	}
	return moves;
}

BitBoard attacks::getPawnAttacks(const BitBoard allPieces, const Color color, const BitBoard square) {
	BitBoard moves = 0UL;
	if (color == WHITE) {
		moves |= square << 7 & allPieces & ~FILES[FILE_H];
		moves |= square << 9 & allPieces & ~FILES[FILE_A];
	} else {
		moves |= square >> 7 & allPieces & ~FILES[FILE_A];
		moves |= square >> 9 & allPieces & ~FILES[FILE_H];
	}
	return moves;
}

BitBoard attacks::getPawnEnPassantAttack(const Color color, const BitBoard enPassant, const BitBoard square) {
	BitBoard moves = 0UL;
	if (color == WHITE) {
		moves |= square << 7 & enPassant & RANKS[RANK_6] & ~FILES[FILE_H];
		moves |= square << 9 & enPassant & RANKS[RANK_6] & ~FILES[FILE_A];
	} else {
		moves |= square >> 7 & enPassant & RANKS[RANK_3] & ~FILES[FILE_A];
		moves |= square >> 9 & enPassant & RANKS[RANK_3] & ~FILES[FILE_H];
	}
	return moves;
}

bool attacks::isSquareAttacked(const Board& board, const Color color, const Square square) {
	//King
	BitBoard attacks = getKingAttacks(SQUARE_MASK[square]);
	if (attacks & board.getBitBoardOfPiece(KING, color))
		return true;
	//Knight
	attacks = getKnightAttacks(SQUARE_MASK[square]);
	if (attacks & board.getBitBoardOfPiece(KNIGHT, color))
		return true;
	//Pawn
	if (color == WHITE)
		attacks = SQUARE_MASK[square] >> 7 & ~FILES[FILE_A] | SQUARE_MASK[square] >> 9 & ~FILES[FILE_H];
	else
		attacks = SQUARE_MASK[square] << 7 & ~FILES[FILE_H] | SQUARE_MASK[square] << 9 & ~FILES[FILE_A];
	if (attacks & board.getBitBoardOfPiece(PAWN, color))
		return true;

	const File file = getFileOf(square);
	const Rank rank = getRankOf(square);
	const BitBoard allPieces = board.getBitBoardOfAllPieces(WHITE) | board.getBitBoardOfAllPieces(BLACK);

	//Bishop
	attacks = getBishopAttacks(allPieces, file, rank, SQUARE_MASK[square]);
	if (attacks & board.getBitBoardOfPiece(BISHOP, color) || attacks & board.getBitBoardOfPiece(QUEEN, color))
		return true;
	//Rook
	attacks = getRookAttacks(allPieces, file, rank, SQUARE_MASK[square]);
	if (attacks & board.getBitBoardOfPiece(ROOK, color) || attacks & board.getBitBoardOfPiece(QUEEN, color))
		return true;
	return false;
}