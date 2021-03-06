#include "BitBoard.h"

static const uShort BITBOARD_FLAGS_EMPTY = 0X0040U;

//0===============================HASHKEY==============================0
BitBoard::Zobrist::Zobrist() {
	for (Square s = A1; s < NONE_SQUARE; ++s)
		for (Piece p = PAWN; p < NONE_PIECE; ++p) {
			pieceKey[s][p][0] = static_cast<uLong>(std::rand()) << 32 | static_cast<uLong>(std::rand()) << 16 | std::rand();
			pieceKey[s][p][1] = static_cast<uLong>(std::rand()) << 32 | static_cast<uLong>(std::rand()) << 16 | std::rand();
		}
	for (File f = FILE_A; f < NONE_FILE; ++f)
		enPassantColumn[f] = static_cast<uLong>(std::rand()) << 32 | static_cast<uLong>(std::rand()) << 16 | std::rand();
	for (uChar i = 0; i < 4; ++i)
		castleKey[i] = static_cast<uLong>(std::rand()) << 32 | static_cast<uLong>(std::rand()) << 16 | std::rand();
	sideKey = static_cast<uLong>(std::rand()) << 32 | static_cast<uLong>(std::rand()) << 16 | std::rand();
}

//0===========================BITBOARD::UNDO===========================0
BitBoard::Undo::Undo() noexcept : move(), flags(BITBOARD_FLAGS_EMPTY), fiftyMove(0U), key(0UL) {}

void BitBoard::Undo::operator()() noexcept {
	move();
	flags = BITBOARD_FLAGS_EMPTY;
	fiftyMove = 0U;
	key = 0UL;
}

Square BitBoard::Undo::getEnPassantSquare() const {
	return static_cast<Square>(flags & 0x00FFU);
}

bool BitBoard::Undo::hasCastlePermission(const CastleFlags castleFlag, const Color color) const {
	return flags >> 8 & 1 << (castleFlag + color);
}

//0=============================BITBOARD===============================0
BitBoard::BitBoard() : key(0UL), flags(BITBOARD_FLAGS_EMPTY), fiftyMove(0U), ply(0U), historyCount(0U), whiteTime(false), hashKeys() {
	for (Piece p = PAWN; p != NONE_PIECE; ++p) {
		bitMaps[p][0] = 0UL;
		bitMaps[p][1] = 0UL;
	}
}

void BitBoard::operator()() noexcept {
	for (Piece p = PAWN; p != NONE_PIECE; ++p) {
		bitMaps[p][0] = 0UL;
		bitMaps[p][1] = 0UL;
	}
	key = 0UL;
	flags = BITBOARD_FLAGS_EMPTY;
	fiftyMove = 0U;
	ply = fiftyMove;
	historyCount = ply;
	whiteTime = false;//checkMate = true;

	for (Undo& h : history)
		h();
}

void BitBoard::setPieceOnSquare(const Piece piece, const Color color, const Square square) {
	bitMaps[piece][color] |= getBitmapOf(square);
	key ^= hashKeys.pieceKey[square][piece][color];
}

void BitBoard::unsetPieceOnSquare(const Piece piece, const Color color, const Square square) {
	bitMaps[piece][color] &= ~getBitmapOf(square);
	key ^= hashKeys.pieceKey[square][piece][color];
}

Piece BitBoard::getPieceFromSquare(const Color color, const Square square) const {
	const Bitmap squareBitmap = getBitmapOf(square);

	Piece piece = PAWN;
	while (piece != NONE_PIECE && (bitMaps[piece][color] & squareBitmap) == 0)
		++piece;

	return piece;
}

Bitmap BitBoard::getBitmapAllPieces(const Color color) const {
	const Bitmap bitmap = bitMaps[PAWN][color] | bitMaps[KNIGHT][color] |
		bitMaps[BISHOP][color] | bitMaps[ROOK][color] |
		bitMaps[QUEEN][color] | bitMaps[KING][color];
	return bitmap;
}

Square BitBoard::getEnPassantSquare() const {
	return static_cast<Square>(flags & 0x00FFU);
}

void BitBoard::setEnPassantSquare(const Square square) {
	if ((flags & 0x00FFU) != NONE_SQUARE)
		key ^= hashKeys.enPassantColumn[getFileOf(getEnPassantSquare())];
	flags = 0xFF00U & flags | square;
	if (square != NONE_SQUARE)
		key ^= hashKeys.enPassantColumn[getFileOf(square)];
}

bool BitBoard::hasCastlePermission(const CastleFlags castleFlag, const Color color) const {
	return flags >> 8 & 1U << (castleFlag + color);
}

void BitBoard::setCastlePermission(const CastleFlags castleFlag, const Color color, const bool permission) {
	if (hasCastlePermission(castleFlag, color) && permission == false) {
		flags &= ~(1U << (castleFlag + color + 8));
		key ^= hashKeys.castleKey[castleFlag + color];
	} else if (!hasCastlePermission(castleFlag, color) && permission == true) {
		flags |= 1U << (castleFlag + color + 8);
		key ^= hashKeys.castleKey[castleFlag + color];
	}
}

bool BitBoard::isRepetition() const {
	for (uShort i = historyCount - fiftyMove; i < historyCount; ++i)
		if (key == history[i].key)
			return true;
	return false;
}

Bitmap BitBoard::getBitmapPiece(const Piece piece, const Color color) const {
	return bitMaps[piece][color];
}

uLong BitBoard::getHashkey() const noexcept {
	return key;
}

uShort BitBoard::getFiftyMove() const noexcept {
	return fiftyMove;
}

uShort BitBoard::getPly() const noexcept {
	return ply;
}

bool BitBoard::isWhiteTime() const noexcept {
	return whiteTime;
}

bool BitBoard::isBlackTime() const noexcept {
	return !whiteTime;
}

Color BitBoard::getColorTime() const noexcept {
	return whiteTime ? WHITE : BLACK;
}

const std::string BitBoard::getFEN() const {
	std::string fen;
	uChar count = 0;
	Square s = NONE_SQUARE;
	Piece p = NONE_PIECE;
	Color c;
	Bitmap allPieces = getBitmapAllPieces(WHITE) | getBitmapAllPieces(BLACK);

	for (Rank r = RANK_8; r != NONE_RANK; --r) {
		for (File f = FILE_A; f < NONE_FILE; ++f) {
			s = getSquareOf(f, r);

			if ((allPieces >> static_cast<uShort>(s)) & 1U) {
				if (count > 0) {
					fen.append(1, count + '0');
					count = 0;
				}

				c = WHITE;
				p = getPieceFromSquare(c, s);
				if (p == NONE_PIECE) {
					c = BLACK;
					p = getPieceFromSquare(c, s);
				}

				fen.append(1, PIECE_CHAR[p][c]);
			} else
				++count;
		}
		if (count > 0) {
			fen.append(1, count + '0');
			count = 0;
		}
		fen.append("/");
	}
	fen.replace(fen.size() - 1, fen.size(), " ");
	fen.append(whiteTime ? "w " : "b ");

	bool castle = false;
	if (hasCastlePermission(KING_CASTLE, WHITE)) {
		fen.append("K");
		castle = true;
	}
	if (hasCastlePermission(QUEEN_CASTLE, WHITE)) {
		fen.append("Q");
		castle = true;
	}
	if (hasCastlePermission(KING_CASTLE, BLACK)) {
		fen.append("k");
		castle = true;
	}
	if (hasCastlePermission(QUEEN_CASTLE, BLACK)) {
		fen.append("q");
		castle = true;
	}
	if (castle == false)
		fen.append("-");

	s = getEnPassantSquare();
	if (s != NONE_SQUARE) {
		fen.append(" ");
		fen.append(1, getFileOf(s) + 'a');
		fen.append(1, getRankOf(s) + '1');
		fen.append(" ");
	} else
		fen.append(" - ");

	fen.append(1, fiftyMove + '0');
	fen.append(" ");
	fen.append(1, ply + '0');

	return fen;
}

void BitBoard::parseFEN(const char* fen) {
	File file = FILE_A;
	Rank rank = RANK_8;
	(*this)();

	while ((rank < NONE_RANK) && *fen) {
		switch (*fen) {
		case 'p': setPieceOnSquare(PAWN, BLACK, getSquareOf(file, rank)); ++file; break;
		case 'n': setPieceOnSquare(KNIGHT, BLACK, getSquareOf(file, rank)); ++file; break;
		case 'b': setPieceOnSquare(BISHOP, BLACK, getSquareOf(file, rank)); ++file; break;
		case 'r': setPieceOnSquare(ROOK, BLACK, getSquareOf(file, rank)); ++file; break;
		case 'q': setPieceOnSquare(QUEEN, BLACK, getSquareOf(file, rank)); ++file; break;
		case 'k': setPieceOnSquare(KING, BLACK, getSquareOf(file, rank)); ++file; break;
		case 'P': setPieceOnSquare(PAWN, WHITE, getSquareOf(file, rank)); ++file; break;
		case 'N': setPieceOnSquare(KNIGHT, WHITE, getSquareOf(file, rank)); ++file; break;
		case 'B': setPieceOnSquare(BISHOP, WHITE, getSquareOf(file, rank)); ++file; break;
		case 'R': setPieceOnSquare(ROOK, WHITE, getSquareOf(file, rank)); ++file; break;
		case 'Q': setPieceOnSquare(QUEEN, WHITE, getSquareOf(file, rank)); ++file; break;
		case 'K': setPieceOnSquare(KING, WHITE, getSquareOf(file, rank)); ++file; break;
		case '1': file = static_cast<File>(file + 1); break;
		case '2': file = static_cast<File>(file + 2); break;
		case '3': file = static_cast<File>(file + 3); break;
		case '4': file = static_cast<File>(file + 4); break;
		case '5': file = static_cast<File>(file + 5); break;
		case '6': file = static_cast<File>(file + 6); break;
		case '7': file = static_cast<File>(file + 7); break;
		case '8': file = static_cast<File>(file + 8); break;
		case '/':
		case ' ':
			--rank;
			file = FILE_A;
			break;
		default:
			std::cerr << "Fen error" << std::endl;
			return;
		}
		++fen;
	}

	//color time
	whiteTime = *fen == 'w';
	if (whiteTime)
		key ^= hashKeys.sideKey;
	fen += 2;

	//castle permission
	if (*fen != '-') {
		for (uChar i = 0; i < 4U; ++i) {
			if (*fen == ' ')
				break;
			else if (*fen == 'K')
				setCastlePermission(KING_CASTLE, WHITE, true);
			else if (*fen == 'Q')
				setCastlePermission(QUEEN_CASTLE, WHITE, true);
			else if (*fen == 'k')
				setCastlePermission(KING_CASTLE, BLACK, true);
			else if (*fen == 'q')
				setCastlePermission(QUEEN_CASTLE, BLACK, true);
			++fen;
		}
	} else
		++fen;

	++fen;
	//en passant
	if (*fen != '-') {
		file = static_cast<File>(*fen - 'a');
		++fen;
		rank = static_cast<Rank>(*fen - '1');
		setEnPassantSquare(getSquareOf(file, rank));
	} else if (*fen)
		fen += 2;

	if (*fen) {
		fiftyMove = *fen - '0';
		fen += 2;
		ply = *fen - '0';
	}
}

std::ostream& operator<<(std::ostream& os, const BitBoard& bitBoard) {
	os << "BitBoard" << std::endl;
	Square s;
	Color c;
	Piece p;
	Rank r;
	File f;

	for (r = RANK_8; r != NONE_RANK; --r) {
		os << r;
		for (f = FILE_A; f < NONE_FILE; ++f) {
			s = getSquareOf(f, r);

			c = WHITE;
			p = bitBoard.getPieceFromSquare(c, s);
			if (p == NONE_PIECE) {
				c = BLACK;
				p = bitBoard.getPieceFromSquare(c, s);
			}

			os << "  ";
			if (p != NONE_PIECE)
				os << PIECE_CHAR[p][c];
			else
				os << '-';
		}
		os << std::endl;
	}
	os << ' ';
	for (f = FILE_A; f < NONE_FILE; ++f)
		os << "  " << f;
	os << std::endl;

	os << "Color Time: " << (bitBoard.whiteTime ? "White " : "Black ");
	os << "Fifty move: " << bitBoard.fiftyMove << " ply: " << bitBoard.ply << std::endl;
	os << "En Passant: " << bitBoard.getEnPassantSquare() << std::endl;
	os << "castle permission: ";
	if (bitBoard.hasCastlePermission(KING_CASTLE, WHITE)) os << 'K'; else os << '-';
	if (bitBoard.hasCastlePermission(QUEEN_CASTLE, WHITE)) os << 'Q'; else os << '-';
	if (bitBoard.hasCastlePermission(KING_CASTLE, BLACK)) os << 'k'; else os << '-';
	if (bitBoard.hasCastlePermission(QUEEN_CASTLE, BLACK)) os << 'q'; else os << '-';
	os << std::endl << "Key: " << bitBoard.key << std::endl;

	return os;
}