#pragma once
#include "Types.h"

class Move {
	/*
	move.flags uint32_t
	00000000 00000000 00000000 11111111 from 0x000000FFU;
	00000000 00000000 11111111 00000000 to 0x0000FF00U;
	00000000 00001111 00000000 00000000 capture 0x000F0000U;
	00000000 11110000 00000000 00000000 promotion piece 0x00F00000U;
	00000001 00000000 00000000 00000000 is pawn start 0x01000000U;
	00000010 00000000 00000000 00000000 is en passant capture 0x02000000U;
	00000100 00000000 00000000 00000000 is castle move 0x04000000U;
	00001000 00000000 00000000 00000000 color move 0x08000000U;
	*/
	uInt flags;

public:
	int score;

	friend std::ostream& operator<<(std::ostream& os, const Move& move);

	Move() noexcept;
	~Move() = default;

	void operator()() noexcept;
	bool isEmpty() const noexcept;

	Square getFrom() const noexcept;
	void setFrom(const Square square) noexcept;

	Square getTo() const noexcept;
	void setTo(const Square square) noexcept;

	Piece getCaptured() const noexcept;
	void setCaptured(const Piece piece) noexcept;

	bool isCapture() const noexcept;

	Piece getPromotionPiece() const noexcept;
	void setPromotionPiece(const Piece piece) noexcept;

	bool isPawnPromotion() const noexcept;

	bool isPawnStart() const noexcept;
	void setPawnStart() noexcept;

	bool isEnPassantCapture() const noexcept;
	void setEnPassantCapture() noexcept;

	bool isCastle() const noexcept;
	void setCastle() noexcept;

	Color getColor() const noexcept;
	void setColor(const Color color) noexcept;

	/*This function create incomplet Move without some flags like:
	* if is capture move
	* if is pawn start
	* if is en passant move
	* if is castle move
	* which color belongs the  move
	*/
	void parseEntry(const char* entry);
};