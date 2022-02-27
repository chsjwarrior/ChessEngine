#pragma once
#include "Types.h"

class Move {
	/* move.flags uint32_t
	00000000 00000000 00000000 11111111 from 0x000000FFU;
	00000000 00000000 11111111 00000000 to 0x0000FF00U;
	00000000 00001111 00000000 00000000 capture 0x000F0000U;
	00000000 11110000 00000000 00000000 promotion piece 0x00F00000U;
	00000001 00000000 00000000 00000000 is pawn start 0x01000000U;
	00000010 00000000 00000000 00000000 is pawn en passant capture 0x02000000U;
	00000100 00000000 00000000 00000000 is pawn promotion 0x04000000U;
	00001000 00000000 00000000 00000000 is king castle move 0x08000000U;
	00010000 00000000 00000000 00000000 is queen castle move 0x10000000U;
	00100000 00000000 00000000 00000000 color move 0x20000000U;
*/
	uInt flags;

public:
	int score;

	friend std::ostream& operator<<(std::ostream& os, const Move& move);

	Move();

	void operator()();
	const bool isEmpty() noexcept;

	const Square getFrom() const;
	void setFrom(const Square square);
	
	const Square getTo() const ;
	void setTo(const Square square);

	const Piece getCaptured() const ;
	void setCaptured(const Piece piece);

	const Piece getPromotionPiece() const;
	void setPromotionPiece(const Piece piece);

	const bool isPawnStart() const;
	void setPawnStart();

	const bool isEnPassantCapture() const;
	void setEnPassantCapture();

	const bool isPawnPromotion() const;
	void setPawnPromotion();

	const bool isKingCastle() const;
	void setKingCastle();

	const bool isQueenCastle() const;
	void setQueenCastle();

	const Color getColor() const;
	void setColor(const Color color);

	/*This function create incomplet Move without some flags like:
	* if is capture move
	* if is pawn start
	* if is en passant move
	* if is king castle move
	* if is queen castle move
	* wich color belongs the  move
	*/
	void parseEntry(const char* entry);
};