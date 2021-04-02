#include <vector>

#include "board.hpp"
#include "fossile_chess.hpp"

FossileChess::FossileChess() {};

Move FossileChess::get_best_move(Board& board) {
	srand(time(NULL));
	std::vector<Move> possible_moves = board.get_all_possible_moves();
	return possible_moves[rand() % possible_moves.size()];
}
