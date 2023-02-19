#pragma once
#include "Chess.h"

/*
also known as Kiwipete by Peter McKenzie.
The number of double-checks in depth 5 is discussed in Talkchess and may be 2645 instead of 2637.

r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -

Fails in depth 4
Depth	Nodes		Captures	E.p.	Castles		Promotions	Checks		Discov. Checks	Dbl. Checks	Checkmates
1		48			8			0		2			0			0			0				0			0
2		2039		351			1		91			0			3			0				0			0
3		97862		17102		45		3162		0			993			0				0			1
4		4085603		757163		1929	128013		15172		25523		42				6			43
5		193690690	35043416	73365	4993637		8392		3309887		19883			2637		30171
6		8031647685	1558445089	3577504	184513607	56627920	92238050	568417			54948		360003
*/

class Uci {
	const char* NAME = "ChessEngine 1.0";
	const char* AUTHOR = "Carlos Henrique Stapait Junior";
	const char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
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
	void uci() const;
	void setOption() const;

public:
	Uci() = default;
	~Uci() = default;

	void loop() const;
};