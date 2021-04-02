#pragma once

#include "board.hpp"

class FossileChess {
	public:
	FossileChess();

	Move get_best_move(Board& board);
};
