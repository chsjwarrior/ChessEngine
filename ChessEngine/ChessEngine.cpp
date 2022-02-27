#include <chrono>
#include "Perft.h"

Move findMove(BitBoard& bitBoard, const char* entry);

int main() {
	std::string imput;
	std::cin >> imput;

	if (imput.compare("uci") != 0) {
		BitBoard b;
		b.parseFEN(START_FEN);
		std::cout << b;

		Move move;
		MoveMaker& moveMaker = MoveMaker::getInstance();

		while (imput.compare("q") != 0) {
			std::cout << "please enter the move >";
			std::cin >> imput;

			if (imput.size() == 1) {
				if (imput.compare("t") == 0) {
					moveMaker.makeUndo(b);
					std::cout << b;
				}
				else if (imput.compare("s") == 0) {
					//info.depth = 5;
					//searchPosition(b);
				}
				else if (imput.compare("p") == 0) {
					do {
						std::cout << "entry with the depth >";
						std::cin >> imput;
					} while (std::ranges::any_of(imput.begin(), imput.end(), [](char c) {return isdigit(c) == 0; }));
					auto start = std::chrono::high_resolution_clock::now();
					perftTest(b, std::stoi(imput));
					auto stop = std::chrono::high_resolution_clock::now();
					auto duration = duration_cast<std::chrono::microseconds>(stop - start);
					std::cout << "Time taken by function: "
						<< duration.count() << " microseconds" << std::endl;
				}
				else if (imput.compare("q") == 0)
					std::cout << "Bye" << std::endl;
			}
			else if (imput.size() >= 4) {
				move = findMove(b, imput.c_str());
				if (!move.isEmpty()) {
					moveMaker.makeMove(b, move);
					if (b.isRepetition())
						std::cout << "is repeating..." << std::endl;
				}
				else
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

Move findMove(BitBoard& bitBoard, const char* entry) {
	Move target;
	target.parseEntry(entry);

	MoveList list;
	MoveGenerator& moveGenerator = MoveGenerator::getInstance();
	moveGenerator.generateMoves(bitBoard, list);
	Piece promotion = NONE_PIECE;

	for (Move* move = list.moves; move != list.moves + list.count; ++move) {
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
			}
			else
				return *move;
		}
	}
	target();
	return target;
}