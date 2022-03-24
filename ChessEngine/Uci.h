#pragma once
#include "Chess.h"

class Uci {
	const char* UCI = "uci";
	const char* SET_OPTION = "setoption";
	const char* POSITION = "position";
	const char* FEN = "fen";
	const char* START_POS = "startpos";
	const char* MOVES = "moves";
	const char* GO = "go";
	const char* SEARCH_MOVES = "searchmoves";
	const char* W_TIME = "wtime";
	const char* B_TIME = "btime";
	const char* W_INC = "winc";
	const char* B_INC = "binc";
	const char* MOVES_TOGO = "movestogo";
	const char* DEPTH = "depth";
	const char* NODES = "nodes";
	const char* MATE = "mate";
	const char* MOVE_TIME = "movetime";
	const char* INFINITE = "infinite";
	const char* QUIT = "quit";
	const char* STOP = "stop";
	const char* IS_READY = "isready";
	const char* UCI_NEW_GAME = "ucinewgame";
	const char* DEBUG = "debug";

	
	void inputPosition(BitBoard& bitBoard, std::istringstream& iss);
	void inputGo(BitBoard& bitBoard, std::istringstream& iss);
	void inputDebugAnalysisTest(BitBoard& bitBoard, std::istringstream& iss);
	void inputUCI();
	void inputSetOption();
	void inputIsReady();
	void inputQuit();

public:
	Uci() = default;
	~Uci() = default;

	const Move findMove(BitBoard& bitBoard, const char* entry);

	void loop();
};