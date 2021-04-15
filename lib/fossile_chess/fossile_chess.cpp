#include <vector>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <thread>

#include "board.hpp"
#include "fossile_chess.hpp"
#include "atomic_hashmap.hpp"
#include "constants.hpp"

#define CHECKMATE 1000000
#define HIGHEST_VALUE (CHECKMATE+1)

static void print_progress(int done, int total) {
	std::cout << "Progress: " << done * 100 / total << "%" << std::endl;
}

void MinimaxThread::run(int depth, HashBoard* board, bool save_all_moves) {
	while (true) {
		Move next_move(-1, -1, -1, -1);
		int num_moves_left;

		// get the next move, our return if we're done
		{
			std::lock_guard<std::mutex> g(master->move_lock);

			num_moves_left = master->moves_left.size();
	
			if (num_moves_left == 0)
				break;

			next_move = master->moves_left.back();
			master->moves_left.pop_back();
		}

		auto total = master->num_moves_total;
		print_progress(total - num_moves_left, total);

		HashBoard b(*board);
		b.move(next_move);

		int eval = master->minimax(&b, depth, -HIGHEST_VALUE, HIGHEST_VALUE, true);
		if (eval < best_move.eval) {
			best_move.move = next_move;
			best_move.eval = eval;
		}

		if (save_all_moves) {
			MoveEval me;
			me.move = next_move;
			me.eval = eval;
			all_moves.push_back(me);
		}
	}
}

static void run_minimax_thread(MinimaxThread* t, int depth, HashBoard* board, bool save_all_moves) {
	t->run(depth, board, save_all_moves);
}

Move FossileChess::get_best_move(Board* board, int depth, int threads_to_use) {
	eval_cache = new AtomicHashmap<BoardEvaluation>(depth * 3 + 3);
	
	HashBoard b(*board);
	std::vector<Move> sorted_moves = b.all_possible_moves;

	// start with low depth and get higher. Sorting after each depth to increase effectivity of alpha beta pruning
	for (int i = 2; i < depth; i++) { 
		std::vector<MoveEval> sorted_move_eval = evaluate_all_moves(sorted_moves, board, i, threads_to_use);
		sorted_moves.clear();
		for (MoveEval& me : sorted_move_eval) {
			sorted_moves.push_back(me.move);
		}
	}

	moves_left = sorted_moves;
	num_moves_total = moves_left.size();

	std::vector<std::thread> threads;
	std::vector<MinimaxThread> threads_data;
	threads.reserve(threads_to_use - 1);
	threads_data.resize(threads_to_use, this);

	// spawn n - 1 threads...
	for (int i = 0; i < threads_to_use - 1; i++)
		threads.emplace_back(run_minimax_thread, &threads_data[i], depth - 1, &b, false);
	// ...since we become the last thread
	run_minimax_thread(&threads_data.back(), depth - 1, &b, false);

	// after we're finished, join all other threads...
	for (std::thread& t : threads)
		t.join();

	// ...and figure out the best move out of all of them
	int best_eval = HIGHEST_VALUE;
	MinimaxThread* best_res = nullptr;
	for (MinimaxThread& res : threads_data) {
		if (res.best_move.eval < best_eval) {
			best_eval = res.best_move.eval;
			best_res = &res;
		}
	}

	// also tell the user that we're done :)
	print_progress(1, 1);

	delete eval_cache;

	return best_res->best_move.move;
}

bool cmp_function(MoveEval m1, MoveEval m2) {return m1.eval < m2.eval;}

std::vector<MoveEval> FossileChess::evaluate_all_moves(std::vector<Move> moves_in, Board* board, int depth, int threads_to_use) {
	HashBoard b(*board);
	moves_left = moves_in;
	num_moves_total = moves_in.size();

	std::vector<std::thread> threads;
	std::vector<MinimaxThread> threads_data;
	threads.reserve(threads_to_use - 1);
	threads_data.resize(threads_to_use, this);

	// spawn n - 1 threads...
	for (int i = 0; i < threads_to_use - 1; i++)
		threads.emplace_back(run_minimax_thread, &threads_data[i], depth - 1, &b, true);
	// ...since we become the last thread
	run_minimax_thread(&threads_data.back(), depth - 1, &b, true);

	// after we're finished, join all other threads...
	for (std::thread& t : threads)
		t.join();

	// ...and sort the moves by their eval
	std::vector<MoveEval> res;
	
	for (MinimaxThread& t : threads_data) {
		for (MoveEval& m : t.all_moves) {
			res.push_back(m);
		}
	}

	std::sort(res.begin(), res.end(), cmp_function);
	
	// also tell the user that we're done :)
	print_progress(1, 1);

	return res;
}

// to initialize this as init-time, use a lambda
static int pieces_values[8] = { 0 };
static int pieces_values_dummy = []() {
	pieces_values[(int)Pieces::Empty] = 0;
	pieces_values[(int)Pieces::King] = 0;
	pieces_values[(int)Pieces::Pawn] = 100;
	pieces_values[(int)Pieces::Knight] = 300;
	pieces_values[(int)Pieces::Bishop] = 310;
	pieces_values[(int)Pieces::Rook] = 500;
	pieces_values[(int)Pieces::Queen] = 900;
	return 0;
} ();

static int get_piece_value(const Piece& p) {
	int mod = p.is_white * 2 - 1;

	return mod * pieces_values[(int)p.type];
}

int FossileChess::evaluate_board(Board* board, int depth_left) { // evaluates from whites perspective
	//TODO: better evaluation (e.g. points for rook on open file etc)
	int eval = 0;

	if (board->gameState == GameState::draw) {
		eval = 0;
		return eval;
	} else if (board->gameState == GameState::white_checkmate) {
		eval = -CHECKMATE - depth_left;
		return eval;
	} else if (board->gameState == GameState::black_checkmate) {
		eval = CHECKMATE + depth_left;
		return eval;
	}

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece p = board->getPiece(x, y);

			eval += get_piece_value(p);

			// special additional points for pawns xd
			if (p.type == (int)Pieces::Pawn) {
				// if the pawn is horizontally in the middle, it gets points for being close to the center
				if ((x > 1 && x < 6) && ((p.is_white && y >= 4 && y >= 5) || (!p.is_white && y <= 3 && y >= 2))) { 
					if (x > 2 && x < 5) { // if it is closer to the center, it gets more points
						eval += (p.is_white) ? (4 /*maximum points*/ - (y - 4)) : -(4 - (3 - y));
					} else {
						eval += (p.is_white) ? (2 /*maximum points*/ - (y - 4)) : -(2 - (3 - y));
					}
				}
			}
		}
	}
	return eval;
}

BoardEvaluation* lookup_eval(AtomicHashmap<BoardEvaluation>* map, uint64_t hash) {
	return map->get(hash, [=](BoardEvaluation* e) {
		return e->hash == hash;
	});
}

int FossileChess::minimax(HashBoard* board, int depth, int alpha, int beta, bool maximizing_player) {
	if (depth == 0 || board->gameState != GameState::playing) {
		return evaluate_board(board, depth);
	}

	int top_eval = HIGHEST_VALUE * (maximizing_player? -1 : 1);

	for (Move m : board->all_possible_moves) {
		HashBoard b = *board;
		b.move(m);

		BoardEvaluation* ev = lookup_eval(eval_cache, b.hash);
		int eval;
		if (ev != nullptr && ev->depth >= depth) {
			eval = ev->eval;
		}
		else {
			eval = minimax(&b, depth - 1, alpha, beta, !maximizing_player);
			eval_cache->insert(b.hash, b.hash, eval, depth);
		}

		if (maximizing_player) {
			top_eval = std::max(top_eval, eval);
			alpha = std::max(alpha, eval);
		}
		else {
			top_eval = std::min(top_eval, eval);
			beta = std::min(beta, eval);
		}
		if (beta <= alpha) {
			break;
		}
	}

	return top_eval;
}

// generator the zobrist table statically so we don't have to worry about it
// at runtime
static constexpr uint64_t do_xorshift(uint64_t x) {
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	return x;
}
static uint64_t zobrist_table[64 * 12];
static int zobrist_table_dummy = []() {
	uint64_t x = 314159;
	// get some initial randomness
	for (int i = 0; i < 10; i++)
		x = do_xorshift(x);

	for (int i = 0; i < 64 * 12; i++) {
		x = do_xorshift(x);
		zobrist_table[i] = x;
	}
	return 0;
} ();

static uint64_t lookup_piece(Piece p, int x, int y) {
	if (p.type == (int)Pieces::Empty)
		return 0;

	int location = y * 8 + x;
	int piece_n = (int)p.type - 1 + (6 * p.is_white);
	return zobrist_table[location * 12 + piece_n];
}

void HashBoard::rebuild_hash() {
	hash = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			Piece p = getPiece(x, y);
			hash ^= lookup_piece(p, x, y);
		}
	}

}

HashBoard::HashBoard(const std::string& fenString) : Board(fenString) {
	rebuild_hash();
}

HashBoard::HashBoard(const Board& other) : Board(other) {
	rebuild_hash();
}

void HashBoard::move(Move move) {
	// TODO castling

	// pieces to remove from the hash
	Piece from = getPiece(move.from_x, move.from_y);
	Piece to = getPiece(move.to_x, move.to_y);
	hash ^= lookup_piece(from, move.from_x, move.from_y);
	hash ^= lookup_piece(to, move.to_x, move.to_y);

	// do the move
	Board::move(move);

	// piece to add to the hash
	Piece new_to = getPiece(move.to_x, move.to_y);
	hash ^= lookup_piece(new_to, move.to_x, move.to_y);
}
