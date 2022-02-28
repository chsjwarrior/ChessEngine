#pragma once
#include "Move.h"

const struct Zobrist {
	uLong pieceKey[NONE_SQUARE][NONE_PIECE][2];
	uLong enPassantColumn[NONE_FILE];
	uLong castleKey[4];
	uLong sideKey;

	Zobrist() noexcept;

	Zobrist(const Zobrist&) = delete;
	Zobrist(Zobrist&&) = delete;
	Zobrist& operator=(const Zobrist&) = delete;
	Zobrist& operator=(Zobrist&&) = delete;
};

class BitBoard {
	class Undo {
		//Move move;
		uShort flags;
		uShort fiftyMove;
		uLong key;

		friend class BitBoard;
		friend class MoveMaker;
	public:
		Undo();
		~Undo() = default;

		Undo(const Undo&) = delete;
		Undo(Undo&&) = delete;
		Undo& operator=(const Undo&) = delete;
		Undo& operator=(Undo&&) = delete;

		void operator()();
		const Square getEnPassantSquare() const;
		const bool hasCastlePermission(const CastleFlags castleFlag, const Color color) const;		
	};

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

	Undo history[MAX_MOVES];//128	

	friend class MoveMaker;
public:
	friend std::ostream& operator<<(std::ostream& os, const BitBoard& bitBoard);
	Zobrist hashKey;
	BitBoard();
	~BitBoard() = default;

	BitBoard(const BitBoard&) = delete;
	BitBoard(BitBoard&&) = delete;
	BitBoard& operator=(const BitBoard&) = delete;
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