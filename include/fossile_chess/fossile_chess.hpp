#pragma once

#include <vector>

#include "board.hpp"

class minimax_thread {
	public:
	minimax_thread(Move m);

	int state = -1; // -1 -> waiting to be processed 0 -> is getting processed 1 -> finished 2 -> realized finish
	int eval = 0;
	Move move = Move(-1, -1, -1, -1);
};

class FossileChess {
	public:
	std::vector<minimax_thread**> moves_to_be_processed;

	FossileChess();

	Move get_best_move(Board* board, int depth, int threads_to_use);
	int evaluate_board(Board* board);
	int minimax(Board* board, int depth, int alpha, int beta, bool maximizing_player);
};
