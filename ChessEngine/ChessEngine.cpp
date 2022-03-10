#include <chrono>
#include "Chess.h"

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

const Move findMove(BitBoard& bitBoard, const char* entry);

int main() {
	std::string imput;
	std::cin >> imput;
	
	if (imput.compare("uci") != 0) {
		BitBoard b;
		b.parseFEN(START_FEN);
		std::cout << b;

		while (imput.compare("q") != 0) {
			std::cout << "please enter the move >";
			std::cin >> imput;

			if (imput.size() == 1) {
				if (imput.compare("t") == 0) {
					MoveMaker::getInstance().makeUndo(b);
					std::cout << b;
				} else if (imput.compare("s") == 0) {
					do {
						std::cout << "entry with the depth >";
						std::cin >> imput;
					} while (std::ranges::any_of(imput.begin(), imput.end(), [](char c) {return isdigit(c) == 0; }));
					info.depth = std::stoi(imput);
					searchPosition(b);
				} else if (imput.compare("p") == 0) {
					do {
						std::cout << "entry with the depth >";
						std::cin >> imput;
					} while (std::ranges::any_of(imput.begin(), imput.end(), [](char c) {return isdigit(c) == 0; }));
					auto start = std::chrono::high_resolution_clock::now();
					perftTest(b, std::stoi(imput));
					auto stop = std::chrono::high_resolution_clock::now();
					auto duration = duration_cast<std::chrono::milliseconds>(stop - start);
					std::cout << "Time taken by function: "
						<< duration.count() << " milliseconds" << std::endl;
				} else if (imput.compare("q") == 0)
					std::cout << "Bye" << std::endl;
			} else if (imput.size() >= 4) {
				Move move = findMove(b, imput.c_str());
				if (!move.isEmpty()) {
					MoveMaker::getInstance().makeMove(b, move);
					if (b.isRepetition())
						std::cout << "is repeating..." << std::endl;
				} else
					std::cerr << imput << " is invalid move." << std::endl;
				std::cout << b;
			}
		}
	}
	//else {
		//Uci uci;
		//uci.loop();
	//}
	return 0;
}

const Move findMove(BitBoard& bitBoard, const char* entry) {
	Move target;
	target.parseEntry(entry);

	Move moves[MAX_MOVES];
	uShort moveCount = moveGenerator::generateMoves(bitBoard, moves);
	Piece promotion = NONE_PIECE;

	for (Move* move = moves; move != moves + moveCount; ++move) {
		if (move->getFrom() == target.getFrom() && move->getTo() == target.getTo()) {
			if (move->isPawnPromotion() && target.isPawnPromotion()) {
				promotion = move->getPromotionPiece();
				if (promotion == QUEEN && target.getPromotionPiece() == promotion)
					return *move;
				else if (promotion == ROOK && target.getPromotionPiece() == promotion)
					return *move;
				else if (promotion == BISHOP && target.getPromotionPiece() == promotion)
					return *move;
				if (promotion == KNIGHT && target.getPromotionPiece() == promotion)
					return *move;
				else {
					target();
					return target;
				}
			} else
				return *move;
		}
	}
	target();
	return target;
}