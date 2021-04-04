#pragma once

#include "board.hpp"

class FossileChess {
	public:
	FossileChess();

	Move get_best_move(Board* board, int depth);
	int evaluate_board(Board* board);
	int minimax(Board* board, int depth, int alpha, int beta, bool maximizing_player);
};
