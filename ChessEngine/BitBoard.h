#pragma once
#include "Move.h"

const struct Zobrist {
	uLong pieceKey[NONE_SQUARE][NONE_PIECE][2];
	uLong enPassantColumn[NONE_FILE];
	uLong castleKey[4];
	uLong sideKey;

	Zobrist() noexcept;

	Zobrist(Zobrist&) = delete;
	Zobrist(Zobrist&&) = delete;
	Zobrist& operator=(Zobrist&) = delete;
	Zobrist& operator=(Zobrist&&) = delete;
} hashKey;

class BitBoard {
	Bitmap bitMaps[6][2];
	uLong key;
	/*
	flags uint16_t
	00000000 11111111 en passant flag 0xFFU;
	00001111 00000000 clastle flag 0xFFU << 8;
	*/
	uShort flags;
	uShort fiftyMove;
	/*
	ply and historyCount musst be separeted counter,
	because FEN notation can load a value of ply but can not load history
	*/
	uShort ply;
	uShort historyCount;
	bool whiteTime;

	struct Undo {
		Move move;
		uShort flags;
		uShort fiftyMove;
		uLong key;

		Undo();

		Undo(Undo&) = delete;
		Undo(Undo&&) = delete;
		Undo& operator=(Undo&) = delete;
		Undo& operator=(Undo&&) = delete;

		void operator()();
		const Square getEnPassantSquare() const;
		const bool hasCastlePermission(const CastleFlags castleFlag, const Color color) const;
	} history[MAX_MOVES];//128

	friend class MoveMaker;
public:
	friend std::ostream& operator<<(std::ostream& os, const BitBoard& bitBoard);

	BitBoard();

	BitBoard(BitBoard&) = delete;
	BitBoard(BitBoard&&) = delete;
	BitBoard& operator=(BitBoard&) = delete;
	BitBoard& operator=(BitBoard&&) = delete;

	void operator()();

	void setPieceOnSquare(const Piece piece, const Color color, const Square square);

	void unsetPieceOnSquare(const Piece piece, const Color color, const Square square);

	const Piece getPieceFromSquare(const Color color, const Square square) const;

	const Bitmap getBitmapAllPieces(const Color color) const;

	const Square getEnPassantSquare() const;

	void setEnPassantSquare(const Square square);

	const bool hasCastlePermission(const CastleFlags castleFlag, const Color color) const;

	void setCastlePermission(const CastleFlags castleFlag, const Color color, const bool permission);

	const bool isRepetition() const;

	const Bitmap getBitmapPiece(const Piece piece, const Color color) const;

	const uLong getHashkey() const;

	const uShort getFiftyMove() const;

	const uShort getPly() const;

	const bool isWhiteTime() const;

	const bool isBlackTime() const;

	const Color getColorTime() const;

	const std::string getFEN() const;

	void parseFEN(const char* fen);
};