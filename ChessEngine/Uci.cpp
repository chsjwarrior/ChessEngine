#include "Uci.h"
#include <thread>

static std::thread* thread = nullptr;

long long Uci::getMilliseconds() const {
	auto now = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
	return duration.count();
}

Move Uci::findMove(const Board& board, const char* entry) const {
	Move target;
	target.parseEntry(entry);

	Move moves[MAX_MOVES];
	uShort moveCount = moveGenerator::generateAllMoves(board, moves);

	for (Move* move = moves; move != moves + moveCount; ++move)
		if (move->getFrom() == target.getFrom() && move->getTo() == target.getTo()) {
			if (move->isPawnPromotion() && target.isPawnPromotion()) {
				Piece promotion = move->getPromotionPiece();
				if (promotion == QUEEN && target.getPromotionPiece() == promotion)
					return *move;
				else if (promotion == ROOK && target.getPromotionPiece() == promotion)
					return *move;
				else if (promotion == BISHOP && target.getPromotionPiece() == promotion)
					return *move;
				else if (promotion == KNIGHT && target.getPromotionPiece() == promotion)
					return *move;
			} else
				return *move;
		}

	target();
	return target;
}

void Uci::position(Board& board, std::istringstream& iss) const {
	std::string input;
	iss >> input;

	if (input == START_POS) {
		board.parseFEN(START_FEN);
		iss >> input;
	} else if (input == FEN) {
		std::string fen;
		while (iss >> input && input != "moves")
			fen += input + " ";
		board.parseFEN(fen.c_str());
	} else
		return;

	if (input == MOVES) {
		Move move;
		while (iss >> input) {
			move = findMove(board, input.c_str());
			if (move.isEmpty())
				break;
			makeMove(board, move);
		}
	}
}

void Uci::go(Board& board, std::istringstream& iss) const {
	std::string input;

	info.depth = -1;
	info.perft = 0;
	info.moveTime = -1;
	info.time = -1;
	info.inc = 0;
	info.movestogo = 30;
	info.nodes = 0UL;
	info.stop = false;

	while (iss >> input) {
		if (input == SEARCH_MOVES)
			while (iss >> input)
				;//limits.searchmoves.push_back(UCI::to_move(pos, token));
		else if (input == MOVE_TIME)
			iss >> info.moveTime;
		else if (input == W_TIME)
			iss >> info.time;
		else if (input == B_TIME)
			iss >> info.time;
		else if (input == W_INC)
			iss >> info.inc;
		else if (input == B_INC)
			iss >> info.inc;
		else if (input == MOVES_TOGO)
			iss >> info.movestogo;
		else if (input == DEPTH)
			iss >> info.depth;
		else if (input == PERFT)
			iss >> info.perft;
		else if (input == MATE)
			iss >> info.mate;
		else if (input == INFINITE)
			iss >> info.infinite;
	}

	if (info.moveTime != -1) {
		info.time = info.moveTime;
		info.movestogo = 1;
	}

	info.startTime = getMilliseconds();

	if (info.time != -1) {
		info.time /= info.movestogo;
		info.time -= 50;
		info.stopTime = info.startTime + info.time + info.inc;
	}

	if (info.depth == -1)
		if (info.perft > 0)
			info.depth = info.perft;
		else
			info.depth = MAX_DEPTH;

	std::cout << "time: " << time;
	std::cout << " start: " << info.startTime;
	std::cout << " stop: " << info.stopTime;
	std::cout << " depth: " << info.depth;
	std::cout << " timeset: ";
	if (info.moveTime == -1)
		std::cout << "false" << std::endl;
	else
		std::cout << "true" << std::endl;

	if (info.perft)
		thread = new std::thread([&board] {
		perftTest(board);
		thread = nullptr;
								 });
	else
		thread = new std::thread([&board] {
		searchPosition(board);
		thread = nullptr;
								 });
}

void Uci::debugAnalysisTest(Board& board, std::istringstream& iss) const {
	const int time = 1140000;
	info.depth = 6;
	info.startTime = getMilliseconds();
	info.stopTime = info.startTime - time;

	std::cout << "time: " << time;
	std::cout << " start: " << info.startTime;
	std::cout << " stop: " << info.stopTime;
	std::cout << " depth: " << info.depth << std::endl;

	thread = new std::thread([&board] {
		searchPosition(board);
		thread = nullptr;
							 });
}

void Uci::uci() const {
	std::cout << "id name " << NAME << std::endl;
	std::cout << "id author " << AUTHOR << std::endl;
	//options go here
	std::cout << "uciok" << std::endl;
}

void Uci::setOption() const {
	//set options
}

void Uci::loop() const {
	Board board;
	board.parseFEN(START_FEN);

	std::string input;
	std::istringstream iss;

	do {
		input.clear();
		std::getline(std::cin, input);

		iss.clear();
		iss.str(input);

		input.clear();
		iss >> std::skipws >> input;

		if (input == POSITION) {
			info.stop = true;
			position(board, iss);
		} else if (input == GO) {
			info.stop = true;
			std::cout << "Seen go.." << std::endl;
			go(board, iss);
		} else if (input == UCI_NEW_GAME) {

		} else if (input == UCI)
			uci();
		else if (input == SET_OPTION)
			setOption();
		else if (input == PRINT)
			std::cout << board << std::endl;
		else if (input == QUIT)
			info.stop = true;
		else if (input == STOP)
			info.stop = true;
		else if (input == IS_READY)
			std::cout << "readyok" << std::endl;

	} while (input != QUIT);
	std::cout << "Bye" << std::endl;
}