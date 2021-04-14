#include <chrono>
#include <iostream>

#include "board.hpp"
#include "fossile_chess.hpp"

int main() {
	Board board;
	FossileChess engine;

	Move m(0, 6, 0, 5);

	auto t1 = std::chrono::high_resolution_clock::now();

	board.move(m);
	Move em = engine.get_best_move(&board, 6, 10);

	auto t2 = std::chrono::high_resolution_clock::now();

	auto time = duration_cast<std::chrono::milliseconds>(t2 - t1);

	std::cout << "Found move: " << (int)em.from_x << "/" << (int)em.from_y
		<< " to " << (int)em.to_x << "/" << (int)em.to_y << std::endl;
	std::cout << "Time used: " << time.count() << "ms" << std::endl;
	return 0;
}
