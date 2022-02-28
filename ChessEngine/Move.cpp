#include "Move.h"

Move::Move() : flags(MOVE_EMPTY), score(0) {}

void Move::operator()() {
	flags = MOVE_EMPTY;
	score = 0;
}

const bool Move::isEmpty() const noexcept {
	return flags == MOVE_EMPTY;
}

const Square Move::getFrom() const {
	return static_cast<Square>(flags & 0xFFU);
}

void Move::setFrom(const Square square) {
	flags = 0xFFFFFF00U & flags | square;
}

const Square Move::getTo() const {
	return static_cast<Square>(flags >> 8 & 0xFFU);
}

void Move::setTo(const Square square) {
	flags = 0xFFFF00FFU & flags | square << 8;
}

const Piece Move::getCaptured() const {
	return static_cast<Piece>(flags >> 16 & 0xFU);
}

void Move::setCaptured(const Piece piece) {
	flags = 0xFFF0FFFFU & flags | piece << 16;
}

const Piece Move::getPromotionPiece() const {
	return static_cast<Piece>(flags >> 20 & 0xFU);
}

void Move::setPromotionPiece(const Piece piece) {
	flags = 0xFF0FFFFFU & flags | piece << 20;
}

const bool Move::isPawnStart() const {
	return hasIntersection(flags, 0x1000000U);
}

void Move::setPawnStart() {
	flags = 0xE0FFFFFFU & flags | 0x1000000U;
}

const bool Move::isEnPassantCapture() const {
	return hasIntersection(flags, 0x2000000U);
}

void Move::setEnPassantCapture() {
	flags = 0xE0FFFFFFU & flags | 0x2000000U;
}

const bool Move::isPawnPromotion() const {
	return hasIntersection(flags, 0x4000000U);
}

void Move::setPawnPromotion() {
	flags = 0xE0FFFFFFU & flags | 0x4000000U;
}

const bool Move::isKingCastle() const {
	return hasIntersection(flags, 0x8000000U);
}

void Move::setKingCastle() {
	flags = 0xE0FFFFFFU & flags | 0x8000000U;
}

const bool Move::isQueenCastle() const {
	return hasIntersection(flags, 0x10000000U);
}

void Move::setQueenCastle() {
	flags = 0xE0FFFFFFU & flags | 0x10000000U;
}

const Color Move::getColor() const {
	return static_cast<Color>(flags >> 29U);
}

void Move::setColor(const Color color) {
	flags = (0xDFFFFFFFU & flags) | static_cast<uInt>(color) << 29U;
}

/*This function create incomplet Move without some flags like:
* if is capture move
* if is pawn start
* if is en passant move
* if is king castle move
* if is queen castle move
* wich color belongs the  move
*/
void Move::parseEntry(const char* entry) {
	(*this)();
	if (entry[0] > 'h' || entry[0] < 'a') {
		std::cerr << "Entry error at index 0" << std::endl;
		return;
	}
	if (entry[1] > '8' || entry[1] < '1') {
		std::cerr << "Entry error at index 1" << std::endl;
		return;
	}
	if (entry[2] > 'h' || entry[2] < 'a') {
		std::cerr << "Entry error at index 2" << std::endl;
		return;
	}
	if (entry[3] > '8' || entry[3] < '1') {
		std::cerr << "Entry error at index 3" << std::endl;
		return;
	}

	const Square from = getSquareOf(static_cast<File>(entry[0] - 'a'), static_cast<Rank>(entry[1] - '1'));
	const Square to = getSquareOf(static_cast<File>(entry[2] - 'a'), static_cast<Rank>(entry[3] - '1'));

	setFrom(from);
	setTo(to);

	if (entry[4] == 'q') {
		setPawnPromotion();
		setPromotionPiece(QUEEN);
	}
	else if (entry[4] == 'r') {
		setPawnPromotion();
		setPromotionPiece(ROOK);
	}
	else if (entry[4] == 'b') {
		setPawnPromotion();
		setPromotionPiece(BISHOP);
	}
	else if (entry[4] == 'n') {
		setPawnPromotion();
		setPromotionPiece(KNIGHT);
	}
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
	if (move.flags != MOVE_EMPTY) {
		os << move.getFrom();
		if (move.getCaptured() != NONE_PIECE || move.isEnPassantCapture())
			os << 'x';
		os << move.getTo();
		if (move.isEnPassantCapture())
			os << "e.p.";
		else if (move.isPawnPromotion())
			os << PIECE_CHAR[move.getPromotionPiece()][move.getColor()];
		else if (move.isKingCastle())
			os << "0-0";
		else if (move.isQueenCastle())
			os << "0-0-0";
	}
	else
		os << "MOVE EMPTY";
	return os;
}