#include "Uci.h"

constexpr char TEST_FEN1[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
/*
TEST_FEN1 does problem in depth 3
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
		b.parseFEN(START_FEN);
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