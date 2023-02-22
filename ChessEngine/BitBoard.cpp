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
	for (uChar i = 0; i < 16; ++i)
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

Square BitBoard::Undo::getEnPassantSquare() const noexcept {
	return static_cast<Square>(flags & 0x00FFU);
}

uChar BitBoard::Undo::getCastlePermission() const noexcept {
	return flags >> 8U;
}

//0=============================BITBOARD===============================0
BitBoard::BitBoard() : ALL_PIECES(6U), key(0UL), flags(BITBOARD_FLAGS_EMPTY), fiftyMove(0U), ply(0U), historyCount(0U), whiteTime(false), hashKeys() {
	for (auto& b : bitMaps) {
		b[0] = 0UL;
		b[1] = 0UL;
	}
}

void BitBoard::operator()() noexcept {
	for (auto& b : bitMaps) {
		b[0] = 0UL;
		b[1] = 0UL;
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

void BitBoard::setPieceOnSquare(const Piece piece, const Color color, const Square square) noexcept {
	const Bitmap squareBitmap = getBitmapOf(square);
	bitMaps[piece][color] |= squareBitmap;
	bitMaps[ALL_PIECES][color] |= squareBitmap;
	key ^= hashKeys.pieceKey[square][piece][color];
}

void BitBoard::unsetPieceOnSquare(const Piece piece, const Color color, const Square square) noexcept {
	const Bitmap squareBitmap = ~getBitmapOf(square);
	bitMaps[piece][color] &= squareBitmap;
	bitMaps[ALL_PIECES][color] &= squareBitmap;
	key ^= hashKeys.pieceKey[square][piece][color];
}

Piece BitBoard::getPieceFromSquare(const Color color, const Square square) const noexcept {
	const Bitmap squareBitmap = getBitmapOf(square);

	if ((bitMaps[ALL_PIECES][color] & squareBitmap) == 0) return NONE_PIECE;

	Piece piece = PAWN;
	while (piece != NONE_PIECE && (bitMaps[piece][color] & squareBitmap) == 0)
		++piece;

	return piece;
}

Bitmap BitBoard::getBitmapAllPieces(const Color color) const noexcept {
	const Bitmap bitmap = bitMaps[PAWN][color] | bitMaps[KNIGHT][color] |
		bitMaps[BISHOP][color] | bitMaps[ROOK][color] |
		bitMaps[QUEEN][color] | bitMaps[KING][color];
	return bitmap;
}

Square BitBoard::getEnPassantSquare() const noexcept {
	return static_cast<Square>(flags & 0x00FFU);
}

void BitBoard::setEnPassantSquare(const Square square) noexcept {
	if ((flags & 0x00FFU) != NONE_SQUARE)
		key ^= hashKeys.enPassantColumn[getFileOf(getEnPassantSquare())];
	flags = 0xFF00U & flags | square;
	if (square != NONE_SQUARE)
		key ^= hashKeys.enPassantColumn[getFileOf(square)];
}

bool BitBoard::hasCastlePermission(const uChar castleFlag) const noexcept {
	return (flags >> 8U & castleFlag) == castleFlag;
}

void BitBoard::setCastlePermission(const uChar castleFlag) noexcept {
	key ^= hashKeys.castleKey[flags >> 8U];
	flags |= castleFlag << 8U;
	key ^= hashKeys.castleKey[flags >> 8U];
}

void BitBoard::unsetCastlePermission(const uChar castleFlag) noexcept {
	key ^= hashKeys.castleKey[flags >> 8U];
	flags &= ~(castleFlag << 8U);
	key ^= hashKeys.castleKey[flags >> 8U];
}

bool BitBoard::isRepetition() const noexcept {
	for (uShort i = historyCount - fiftyMove; i < historyCount; ++i)
		if (key == history[i].key)
			return true;
	return false;
}

Bitmap BitBoard::getBitmapPiece(const Piece piece, const Color color) const noexcept {
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

				fen.append(1, PIECE_CHAR[p + c * 6]);
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
	if (hasCastlePermission(WHITE_KING_CASTLE)) {
		fen.append("K");
		castle = true;
	}
	if (hasCastlePermission(WHITE_QUEEN_CASTLE)) {
		fen.append("Q");
		castle = true;
	}
	if (hasCastlePermission(BLACK_KING_CASTLE)) {
		fen.append("k");
		castle = true;
	}
	if (hasCastlePermission(BLACK_QUEEN_CASTLE)) {
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
		uChar castleFlag = 0U;
		for (uChar i = 0; i < 4U; ++i) {
			if (*fen == ' ')
				break;
			else if (*fen == 'K')
				castleFlag |= WHITE_KING_CASTLE;
			else if (*fen == 'Q')
				castleFlag |= WHITE_QUEEN_CASTLE;
			else if (*fen == 'k')
				castleFlag |= BLACK_KING_CASTLE;
			else if (*fen == 'q')
				castleFlag |= BLACK_QUEEN_CASTLE;
			++fen;
		}
		key ^= hashKeys.castleKey[flags >> 8U];
		setCastlePermission(castleFlag);
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
				os << PIECE_CHAR[p + c * 6];
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
	if (bitBoard.hasCastlePermission(WHITE_KING_CASTLE)) os << 'K'; else os << '-';
	if (bitBoard.hasCastlePermission(WHITE_QUEEN_CASTLE)) os << 'Q'; else os << '-';
	if (bitBoard.hasCastlePermission(BLACK_KING_CASTLE)) os << 'k'; else os << '-';
	if (bitBoard.hasCastlePermission(BLACK_QUEEN_CASTLE)) os << 'q'; else os << '-';
	os << std::endl << "Key: " << bitBoard.key << std::endl;

	return os;
}