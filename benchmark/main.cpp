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
	Move em = engine.get_best_move(&board, 5, 1);
	
	auto t2 = std::chrono::high_resolution_clock::now();
	
	auto time = duration_cast<std::chrono::milliseconds>(t2 - t1);
	
	std::cout << "Found move: " << em.from_x << " " << em.from_y << " to " << em.to_x << " " << em.to_y << ", time used: " << time.count() << std::endl;
	return 0;
}
