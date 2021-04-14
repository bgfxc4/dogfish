#pragma once

#include <mutex>
#include <vector>
#include <stdint.h>

#include "board.hpp"
#include "atomic_hashmap.hpp"

class FossileChess;
class HashBoard;

class BoardEvaluation {
	public:
	uint64_t hash;
	int eval;
	int depth;

	BoardEvaluation(uint64_t _hash, int _eval, int _depth)
		: hash(_hash), eval(_eval), depth(_depth) {}
};

class MoveEval {
	public:
	Move move = Move(-1, -1, -1, -1);
	int eval = 999999999;
	bool operator<(MoveEval& other) { return eval < other.eval; }
};

class MinimaxThread {
	public:
	FossileChess* master;
	MoveEval best_move;
	std::vector<MoveEval> all_moves; // only used for iterative search, otherwise unused

	MinimaxThread(FossileChess* _master) : master(_master) {}

	void run(int depth, HashBoard* board, bool save_all_moves);
};

class FossileChess {
	public:
	std::mutex move_lock;
	std::vector<Move> moves_left;
	AtomicHashmap<BoardEvaluation>* eval_cache = nullptr;
	int num_moves_total;

	FossileChess() {}
	FossileChess(const FossileChess& other) = delete;

	Move get_best_move(Board* board, int depth, int threads_to_use);
	std::vector<MoveEval> evaluate_all_moves(std::vector<Move> moves_in, HashBoard* board, int depth, int threads_to_use);
	static int evaluate_board(Board* board, int depth_left);
	int minimax(HashBoard* board, int depth, int alpha, int beta, bool maximizing_player);
};

class HashBoard : public Board {
	public:
	uint64_t hash;

	HashBoard(const std::string& fenString);
	HashBoard() : HashBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {}
	HashBoard(const Board& other);

	void move(Move move);

	private:
	void rebuild_hash();
};
