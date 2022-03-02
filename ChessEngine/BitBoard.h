#pragma once
#include "Move.h"

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
	because FEN notation can load a value of ply but can not load history count
	*/
	uShort ply;
	uShort historyCount;
	bool whiteTime;

	struct Zobrist {
		uLong pieceKey[NONE_SQUARE][NONE_PIECE][2];
		uLong enPassantColumn[NONE_FILE];
		uLong castleKey[4];
		uLong sideKey;

		Zobrist();
		~Zobrist() = default;
	} hashKeys;

	struct Undo {
		Move move;
		uShort flags;
		uShort fiftyMove;
		uLong key;

		Undo();
		~Undo() = default;

		void operator()();
		Square getEnPassantSquare() const;
		bool hasCastlePermission(const CastleFlags castleFlag, const Color color) const;
	} history[MAX_MOVES];//128		

	friend class MoveMaker;
public:
	friend std::ostream& operator<<(std::ostream& os, const BitBoard& bitBoard);

	BitBoard();
	~BitBoard() = default;

	BitBoard(const BitBoard&) = delete;
	BitBoard(BitBoard&&) = delete;
	BitBoard& operator=(const BitBoard&) = delete;
	BitBoard& operator=(BitBoard&&) = delete;

	void operator()();

	void setPieceOnSquare(const Piece piece, const Color color, const Square square);

	void unsetPieceOnSquare(const Piece piece, const Color color, const Square square);

	Piece getPieceFromSquare(const Color color, const Square square) const;

	Bitmap getBitmapAllPieces(const Color color) const;

	Square getEnPassantSquare() const;

	void setEnPassantSquare(const Square square);

	bool hasCastlePermission(const CastleFlags castleFlag, const Color color) const;

	void setCastlePermission(const CastleFlags castleFlag, const Color color, const bool permission);

	bool isRepetition() const;

	Bitmap getBitmapPiece(const Piece piece, const Color color) const;

	uLong getHashkey() const;

	uShort getFiftyMove() const;

	uShort getPly() const;

	bool isWhiteTime() const;

	bool isBlackTime() const;

	Color getColorTime() const;

	const std::string getFEN() const;
	/* This funcion still needs to be improve on read fifty move and ply*/
	void parseFEN(const char* fen);
};