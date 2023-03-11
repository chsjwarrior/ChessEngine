#pragma once
#include "Move.h"
#include "BitBoard.h"

class Board {
	BitBoard bitMaps[7][2];// The last position ist the union of all previous
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

	const struct Zobrist {
		uLong pieceKey[NONE_SQUARE][NONE_PIECE][2];
		uLong enPassantColumn[NONE_FILE];
		uLong castleKey[16];
		uLong sideKey;

		Zobrist();
		~Zobrist() = default;

		Zobrist(Zobrist&) = delete;
		Zobrist(Zobrist&&) = delete;
		Zobrist& operator=(Zobrist&) = delete;
		Zobrist& operator=(Zobrist&&) = delete;
	} hashKeys;

	struct Undo {
		Move move;
		uShort flags;
		uShort fiftyMove;
		uLong key;

		Undo() noexcept;
		~Undo() = default;

		Undo(Undo&) = delete;
		Undo(Undo&&) = delete;
		Undo& operator=(Undo&) = delete;
		Undo& operator=(Undo&&) = delete;

		void operator()() noexcept;
		Square getEnPassantSquare() const noexcept;
		uChar getCastlePermission() const noexcept;
	} history[MAX_MOVES];

	friend void makeUndo(Board& board);
	friend bool makeMove(Board& board, const Move& move);
	friend void searchPosition(Board& board);
public:
	friend std::ostream& operator<<(std::ostream& os, const Board& board);

	Board();
	~Board() = default;

	Board(const Board&) = delete;
	Board(Board&&) = delete;
	Board& operator=(const Board&) = delete;
	Board& operator=(Board&&) = delete;

	void operator()() noexcept;

	void setPieceOnSquare(const Piece piece, const Color color, const Square square) noexcept;

	void unsetPieceOnSquare(const Piece piece, const Color color, const Square square) noexcept;

	Piece getPieceFromSquare(const Color color, const Square square) const noexcept;

	BitBoard getBitBoardOfAllPieces(const Color color) const noexcept;

	Square getEnPassantSquare() const noexcept;

	void setEnPassantSquare(const Square square) noexcept;

	bool hasCastlePermission(const uChar castleFlag) const noexcept;

	void setCastlePermission(const uChar castleFlag) noexcept;

	void unsetCastlePermission(const uChar castleFlag) noexcept;

	bool isRepetition() const noexcept;

	BitBoard getBitBoardOfPiece(const Piece piece, const Color color) const noexcept;

	uLong getHashkey() const noexcept;

	uShort getFiftyMove() const noexcept;

	uShort getPly() const noexcept;

	bool isWhiteTime() const noexcept;

	bool isBlackTime() const noexcept;

	Color getColorTime() const noexcept;

	const std::string getFEN() const;
	/* This funcion still needs to be improve on read fifty move and ply */
	void parseFEN(const char* fen);
};