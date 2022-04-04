#include "Uci.h"

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

int main() {
	std::string input;
	std::cin >> input;

	if (input.compare("uci") == 0) {
		Uci uci;
		//uci.loop();

		BitBoard b;
		b.parseFEN(TEST_FEN1);
		std::cout << b;

		while (input.compare("q") != 0) {
			std::cout << "please enter the move >";
			std::cin >> input;

			if (input.size() == 1) {
				if (input.compare("t") == 0) {
					MoveMaker::getInstance().makeUndo(b);
					std::cout << b;
				} else if (input.compare("s") == 0) {
					do {
						std::cout << "entry with the depth >";
						std::cin >> input;
					} while (std::ranges::any_of(input.begin(), input.end(), [](char c) {return isdigit(c) == 0; }));
					info.depth = std::stoi(input);
					info.startTime = uci.getMilliseconds();
					info.stopTime = info.startTime + 500000000;
					std::cout << "start: " << info.startTime << std::endl;
					std::cout << "stop: " << info.stopTime << std::endl;
					searchPosition(b);
				} else if (input.compare("p") == 0) {
					do {
						std::cout << "entry with the depth >";
						std::cin >> input;
					} while (std::ranges::any_of(input.begin(), input.end(), [](char c) {return isdigit(c) == 0; }));
					auto start = std::chrono::high_resolution_clock::now();
					perftTest(b, std::stoi(input));
					auto stop = std::chrono::high_resolution_clock::now();
					auto duration = duration_cast<std::chrono::milliseconds>(stop - start);
					std::cout << "Time taken by function: " << duration.count() << " milliseconds" << std::endl;					
				} else if (input.compare("f") == 0) {
					std::cout << "entry with the FEN >";
					std::cin >> input;
					b.parseFEN(input.c_str());
				} else if (input.compare("q") == 0)
					std::cout << "Bye" << std::endl;
			} else if (input.size() >= 4) {
				Move move = uci.findMove(b, input.c_str());
				if (!move.isEmpty()) {
					MoveMaker::getInstance().makeMove(b, move);
					if (b.isRepetition())
						std::cout << "is repeating..." << std::endl;
				} else
					std::cerr << input << " is invalid move." << std::endl;
				std::cout << b;
			}
		}
	}
	return 0;
}