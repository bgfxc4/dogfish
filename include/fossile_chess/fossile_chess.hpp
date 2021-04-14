#pragma once

#include <mutex>
#include <vector>
#include <stdint.h>

#include "board.hpp"

class FossileChess;

class MinimaxThread {
	public:
	FossileChess* master;
	Move best_move = Move(-1, -1, -1, -1);
	int best_move_eval = 999999999;

	MinimaxThread(FossileChess* _master) : master(_master) {}

	void run(int depth, Board* board);
};

class FossileChess {
	public:
	std::mutex move_lock;
	std::vector<Move> moves_left;
	int num_moves_total;

	FossileChess() {}
	FossileChess(const FossileChess& other) = delete;

	Move get_best_move(Board* board, int depth, int threads_to_use);
	static int evaluate_board(Board* board, int depth_left);
	static int minimax(Board* board, int depth, int alpha, int beta, bool maximizing_player);
};

class HashBoard : Board {
	public:
	uint64_t hash;

	HashBoard(const std::string& fenString);
	HashBoard() : HashBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {}

	void move(Move move);
};
