#include "Move.h"

static const uInt MOVE_EMPTY = 0x00664040U;

Move::Move() noexcept : flags(MOVE_EMPTY), score(0) {}

void Move::operator()() noexcept {
	flags = MOVE_EMPTY;
	score = 0;
}

bool Move::operator==(const Move& other) const noexcept {
	return flags == other.flags;
}

bool Move::isEmpty() const noexcept {
	return flags == MOVE_EMPTY;
}

Square Move::getFrom() const noexcept {
	return static_cast<Square>(flags & 0xFFU);
}

void Move::setFrom(const Square square) noexcept {
	flags = 0xFFFFFF00U & flags | square;
}

Square Move::getTo() const noexcept {
	return static_cast<Square>(flags >> 8 & 0xFFU);
}

void Move::setTo(const Square square) noexcept {
	flags = 0xFFFF00FFU & flags | square << 8;
}

Piece Move::getCaptured() const noexcept {
	return static_cast<Piece>(flags >> 16 & 0xFU);
}

void Move::setCaptured(const Piece piece) noexcept {
	flags = 0xFFF0FFFFU & flags | piece << 16;
}

bool Move::isCapture() const noexcept {
	return (flags >> 16 & 0xFU) != NONE_PIECE;
}

Piece Move::getPromotionPiece() const noexcept {
	return static_cast<Piece>(flags >> 20 & 0xFU);
}

void Move::setPromotionPiece(const Piece piece) noexcept {
	if (piece > PAWN && piece < KING)
		flags = 0xFF0FFFFFU & flags | piece << 20;
}

bool Move::isPawnPromotion() const noexcept {
	return (flags >> 20 & 0xFU) != NONE_PIECE;
}

bool Move::isPawnStart() const noexcept {
	return flags & 0x1000000U;
}

void Move::setPawnStart() noexcept {
	flags = 0xF8FFFFFFU & flags | 0x1000000U;
}

bool Move::isEnPassantCapture() const noexcept {
	return flags & 0x2000000U;
}

void Move::setEnPassantCapture() noexcept {
	flags = 0xF8FFFFFFU & flags | 0x2000000U;
}

bool Move::isCastle() const noexcept {
	return flags & 0x4000000U;
}

void Move::setCastle() noexcept {
	flags = 0xF8FFFFFFU & flags | 0x4000000U;
}

Color Move::getColor() const noexcept {
	return static_cast<Color>(flags >> 27);
}

void Move::setColor(const Color color) noexcept {
	flags = 0xF7FFFFFFU & flags | static_cast<uInt>(color) << 27;
}

void Move::parseEntry(const char* entry) {
	(*this)();

	const size_t length = strlen(entry);
	if (length < 4) {
		std::cerr << "Move::parseNetry() Entry size: " << length << std::endl;
		return;
	}

	if (entry[0] > 'h' || entry[0] < 'a') {
		std::cerr << "Move::parseNetry() Entry error at index 0" << std::endl;
		return;
	}
	if (entry[1] > '8' || entry[1] < '1') {
		std::cerr << "Move::parseNetry() Entry error at index 1" << std::endl;
		return;
	}
	if (entry[2] > 'h' || entry[2] < 'a') {
		std::cerr << "Move::parseNetry() Entry error at index 2" << std::endl;
		return;
	}
	if (entry[3] > '8' || entry[3] < '1') {
		std::cerr << "Move::parseNetry() Entry error at index 3" << std::endl;
		return;
	}

	setFrom(getSquareOf(static_cast<File>(entry[0] - 'a'), static_cast<Rank>(entry[1] - '1')));
	setTo(getSquareOf(static_cast<File>(entry[2] - 'a'), static_cast<Rank>(entry[3] - '1')));

	if (length == 5) { // if (entry[4] != '\0')
		switch (entry[4]) {
		case 'q':
			setPromotionPiece(QUEEN);
			break;
		case 'r':
			setPromotionPiece(ROOK);
			break;
		case 'b':
			setPromotionPiece(BISHOP);
			break;
		case 'n':
			setPromotionPiece(KNIGHT);
		}
	}
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
	if (move.flags != MOVE_EMPTY) {
		os << move.getFrom();
		os << move.getTo();
		if (move.isPawnPromotion())
			os << PIECE_CHAR[move.getPromotionPiece() + move.getColor() * 6];
	} else
		os << "0000";// MOVE EMPTY
	return os;
}