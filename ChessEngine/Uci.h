#pragma once
#include "Chess.h"

//--Short Castling Gives Check
constexpr char TEST_CASTLING1[] = "5k2/8/8/8/8/8/8/4K2R w K - 0 1";// perft 6 = 661072 
//--Long Castling Gives Check
constexpr char TEST_CASTLING2[] = "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1";// perft 6 = 803711
//--Castle Rights
constexpr char TEST_CASTLE_RIGHTS[] = "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1";// perft 4 = 1274206
//--Castling Prevented
constexpr char TEST_CASTLE_PREVENTED[] = "r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1";// perft 4 = 1720476

constexpr char TEST_FEN1[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
/*
D	N
1	44
2	1,486
3	62,379
4	2,103,487
5	89,941,194
*/

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
	const char* PERFT = "perft";
	const char* NODES = "nodes";
	const char* MATE = "mate";
	const char* MOVE_TIME = "movetime";
	const char* INFINITE = "infinite";
	const char* QUIT = "quit";
	const char* STOP = "stop";
	const char* IS_READY = "isready";
	const char* UCI_NEW_GAME = "ucinewgame";
	const char* PRINT = "print";

	long long getMilliseconds() const;
	Move findMove(const BitBoard& bitBoard, const char* entry) const;
	void position(BitBoard& bitBoard, std::istringstream& iss) const;
	void go(BitBoard& bitBoard, std::istringstream& iss) const;
	void debugAnalysisTest(BitBoard& bitBoard, std::istringstream& iss) const;
	void print(const BitBoard& bitBoard) const;
	void uci() const;
	void setOption() const;
	void isReady() const;
	void quit() const;

public:
	Uci() = default;
	~Uci() = default;

	void loop() const;
};