#pragma once

#include <mutex>
#include <vector>
#include <stdint.h>

#include "board.hpp"
#include "atomic_hashmap.hpp"

class FossileChess;
class HashBoard;

struct MoveEval {
	Move m;
	int eval;

	MoveEval(const Move& _m, int _eval) : m(_m), eval(_eval) {}
	bool operator<(const MoveEval& other) { return eval < other.eval; }
};

class BoardEvaluation {
	public:
	uint64_t hash;
	int eval;
	int depth;

	BoardEvaluation(uint64_t _hash, int _eval, int _depth)
		: hash(_hash), eval(_eval), depth(_depth) {}
};

class MinimaxThread {
	public:
	FossileChess* master;
	Move best_move = Move(-1, -1, -1, -1);
	int best_move_eval = 999999999;

	MinimaxThread(FossileChess* _master) : master(_master) {}

	void run(int depth, HashBoard* board);
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
	static int evaluate_board(Board* board, int depth_left);
	int minimax(HashBoard* board, int depth, bool maximizing_player);

	private:
	int minimax(HashBoard* board, int depth, int alpha, int beta, bool maximizing_player);
	void minimax(std::vector<MoveEval>& moves, HashBoard* board,
			int depth, int alpha, int beta, bool maximizing_player);
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
