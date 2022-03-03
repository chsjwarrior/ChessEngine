#include "BitBoard.h"

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
BitBoard::Undo::Undo() : move(), flags(BITBOARD_FLAGS_EMPTY), fiftyMove(0U), key(0UL) {}

void BitBoard::Undo::operator()() {
	move();
	flags = BITBOARD_FLAGS_EMPTY;
	fiftyMove = 0U;
	key = 0UL;
}

Square BitBoard::Undo::getEnPassantSquare() const {
	return static_cast<Square>(flags & 0x00FFU);
}

bool BitBoard::Undo::hasCastlePermission(const CastleFlags castleFlag, const Color color) const {
	return hasIntersection(flags >> 8, 1 << (castleFlag + color));
}

//0=============================BITBOARD===============================0
BitBoard::BitBoard() : key(0UL), flags(BITBOARD_FLAGS_EMPTY), fiftyMove(0U), ply(0U), historyCount(0U), whiteTime(false), hashKeys() {
	for (Piece p = PAWN; p != NONE_PIECE; ++p) {
		bitMaps[p][0] = 0UL;
		bitMaps[p][1] = 0UL;
	}
}

void BitBoard::operator()() {
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
	bitMaps[piece][color] = getUnion(bitMaps[piece][color], getBitmapOf(square));
	key ^= hashKeys.pieceKey[square][piece][color];
}

void BitBoard::unsetPieceOnSquare(const Piece piece, const Color color, const Square square) {
	bitMaps[piece][color] = unsetIntersections(bitMaps[piece][color], getBitmapOf(square));
	key ^= hashKeys.pieceKey[square][piece][color];
}

Piece BitBoard::getPieceFromSquare(const Color color, const Square square) const {
	const Bitmap bitmap = getBitmapOf(square);
	Piece piece = NONE_PIECE;

	for (Piece p = PAWN; p != NONE_PIECE && piece == NONE_PIECE; ++p)
		if ((bitMaps[p][color] & bitmap) != 0)
			piece = p;

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
	if (getEnPassantSquare() != NONE_SQUARE)
		key ^= hashKeys.enPassantColumn[getFileOf(getEnPassantSquare())];
	flags = 0xF00U & flags | square;
	if (square != NONE_SQUARE)
		key ^= hashKeys.enPassantColumn[getFileOf(square)];
}

bool BitBoard::hasCastlePermission(const CastleFlags castleFlag, const Color color) const {
	return hasIntersection(flags >> 8, 1 << (castleFlag + color));
}

void BitBoard::setCastlePermission(const CastleFlags castleFlag, const Color color, const bool permission) {
	const int index = castleFlag + color;
	if (hasCastlePermission(castleFlag, color) && permission == false) {
		key ^= hashKeys.castleKey[index];
		flags = unsetIntersections<uShort>(flags, 1 << (index + 8));
	} else if (!hasCastlePermission(castleFlag, color) && permission == true) {
		key ^= hashKeys.castleKey[index];
		flags = getUnion<uShort>(flags, 1 << (index + 8));
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

uLong BitBoard::getHashkey() const {
	return key;
}

uShort BitBoard::getFiftyMove() const {
	return fiftyMove;
}

uShort BitBoard::getPly() const {
	return ply;
}

bool BitBoard::isWhiteTime() const {
	return whiteTime;
}

bool BitBoard::isBlackTime() const {
	return !whiteTime;
}

Color BitBoard::getColorTime() const {
	return whiteTime ? WHITE : BLACK;
}

const std::string BitBoard::getFEN() const {
	std::string fen;
	uChar count = 0;
	Square s = NONE_SQUARE;
	Piece p = NONE_PIECE;
	Color c;
	Bitmap allPieces = getUnion(getBitmapAllPieces(WHITE), getBitmapAllPieces(BLACK));

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
	uShort count = 0;
	Piece piece = NONE_PIECE;
	Color color;
	(*this)();

	while ((rank < NONE_RANK) && *fen) {
		count = 1;
		switch (*fen) {
		case 'p': piece = PAWN; color = BLACK; break;
		case 'n': piece = KNIGHT; color = BLACK; break;
		case 'b': piece = BISHOP; color = BLACK; break;
		case 'r': piece = ROOK; color = BLACK; break;
		case 'q': piece = QUEEN; color = BLACK; break;
		case 'k': piece = KING; color = BLACK; break;
		case 'P': piece = PAWN; color = WHITE; break;
		case 'R': piece = ROOK; color = WHITE; break;
		case 'N': piece = KNIGHT; color = WHITE; break;
		case 'B': piece = BISHOP; color = WHITE; break;
		case 'K': piece = KING; color = WHITE; break;
		case 'Q': piece = QUEEN; color = WHITE; break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			piece = NONE_PIECE;
			count = *fen - '0';
			break;
		case '/':
		case ' ':
			--rank;
			file = FILE_A;
			++fen;
			continue;
		default:
			std::cerr << "Fen error" << std::endl;
			return;
		}

		for (uShort i = 0; i < count; ++i) {
			if (piece != NONE_PIECE)
				setPieceOnSquare(piece, color, getSquareOf(file, rank));
			++file;
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
		if (*fen == 'K') {
			setCastlePermission(KING_CASTLE, WHITE, true);
			++fen;
		}
		if (*fen == 'Q') {
			setCastlePermission(QUEEN_CASTLE, WHITE, true);
			++fen;
		}
		if (*fen == 'k') {
			setCastlePermission(KING_CASTLE, BLACK, true);
			++fen;
		}
		if (*fen == 'q') {
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
	if (bitBoard.hasCastlePermission(KING_CASTLE, WHITE))
		os << 'K';
	if (bitBoard.hasCastlePermission(QUEEN_CASTLE, WHITE))
		os << 'Q';
	if (bitBoard.hasCastlePermission(KING_CASTLE, BLACK))
		os << 'k';
	if (bitBoard.hasCastlePermission(QUEEN_CASTLE, BLACK))
		os << 'q';
	os << std::endl << "Key: " << bitBoard.key << std::endl;

	return os;
}