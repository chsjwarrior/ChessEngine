#include "Uci.h"

static long long getMilliseconds() {
	auto now = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
	return duration.count();
}

void Uci::inputPosition(BitBoard& bitBoard, std::istringstream& iss) {
	std::string input;
	iss >> input;

	if (input == FEN) {
		std::string fen;
		while (iss >> input && input != "moves")
			fen += input + " ";
		bitBoard.parseFEN(fen.c_str());
	} else if (input == START_POS) {
		bitBoard.parseFEN(START_FEN);
		iss >> input;
	} else
		return;

	MoveMaker& moveMaker = MoveMaker::getInstance();
	Move move;
	while (true) {
		iss >> input;
		move.parseEntry(input.c_str());
		if (move.isEmpty())
			break;
		moveMaker.makeMove(bitBoard, move);
	}
}

void Uci::inputGo(BitBoard& bitBoard, std::istringstream& iss) {
	std::string input;

	info.depth = -1;
	info.moveTime = -1;
	info.time = -1;
	info.inc = 0;
	info.movestogo = 30;

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

	if (info.depth)
		info.depth = MAX_DEPTH;

	std::cout << "time: " << time;
	std::cout << "start: " << info.startTime;
	std::cout << "stop: " << info.stopTime;
	std::cout << "depth: " << info.depth;
	std::cout << "timeset: ";
	if (info.moveTime == -1)
		std::cout << "false" << std::endl;
	else
		std::cout << "true" << std::endl;

	searchPosition(bitBoard);
}

void Uci::inputDebugAnalysisTest(BitBoard& bitBoard, std::istringstream& iss) {
	const int time = 1140000;
	info.depth = 6;
	info.startTime = getMilliseconds();
	info.stopTime = info.startTime - time;

	std::cout << "time: " << time;
	std::cout << " start: " << info.startTime;
	std::cout << " stop: " << info.stopTime;
	std::cout << " depth: " << info.depth << std::endl;

	searchPosition(bitBoard);
}

void Uci::inputUCI() {
	std::cout << "id name " << NAME << std::endl;
	std::cout << "id author " << AUTHOR << std::endl;
	//options go here
	std::cout << "uciok" << std::endl;
}

void Uci::inputSetOption() {
	//set options
}

void Uci::inputIsReady() {
	std::cout << "readyok" << std::endl;
}

void Uci::inputQuit() {
	info.stop = true;
	std::cout << "Bye" << std::endl;
}

void Uci::loop() {
	BitBoard bitBoard;

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
			inputPosition(bitBoard, iss);
			std::cout << bitBoard;
		} else if (input == GO) {
			std::cout << "Seen go.." << std::endl;
			inputGo(bitBoard, iss);
		} else if (input == UCI_NEW_GAME) {
			iss.str("position startpos\n");
			inputPosition(bitBoard, iss);
		} else if (input == UCI)
			inputUCI();
		else if (input == SET_OPTION)
			inputSetOption();
		else if (input == DEBUG)
			inputDebugAnalysisTest(bitBoard, iss);
		else if (input == QUIT)
			inputQuit();
		else if (input == STOP)
			info.stop = true;
		else if (input == IS_READY)
			inputIsReady();

	} while (input != "quit");
}