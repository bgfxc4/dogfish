#include <vector>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <thread>

#include "board.hpp"
#include "fossile_chess.hpp"
#include "constants.hpp"

#define CHECKMATE 1000000
#define HIGHEST_VALUE (CHECKMATE+1)

static void print_progress(int done, int total) {
	std::cout << "Progress: " << done * 100 / total << "%" << std::endl;
}

void MinimaxThread::run(int depth, Board* board) {
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

		Board b = *board;
		b.move(next_move);

		int eval = FossileChess::minimax(&b, depth, -HIGHEST_VALUE, HIGHEST_VALUE, true);
		if (eval < best_move_eval) {
			best_move = next_move;
			best_move_eval = eval;
		}
	}
}

static void run_minimax_thread(MinimaxThread* t, int depth, Board* board) {
	t->run(depth, board);
}

Move FossileChess::get_best_move(Board* board, int depth, int threads_to_use) {
	moves_left = board->all_possible_moves;
	num_moves_total = board->all_possible_moves.size();

	std::vector<std::thread> threads;
	std::vector<MinimaxThread> threads_data;
	threads.reserve(threads_to_use - 1);
	threads_data.resize(threads_to_use, this);

	// spawn n - 1 threads...
	for (int i = 0; i < threads_to_use - 1; i++)
		threads.emplace_back(run_minimax_thread, &threads_data[i], depth - 1, board);
	// ...since we become the last thread
	run_minimax_thread(&threads_data.back(), depth - 1, board);

	// after we're finished, join all other threads...
	for (std::thread& t : threads)
		t.join();

	// ...and figure out the best move out of all of them
	int best_eval = HIGHEST_VALUE;
	MinimaxThread* best_res = nullptr;
	for (MinimaxThread& res : threads_data) {
		if (res.best_move_eval < best_eval) {
			best_eval = res.best_move_eval;
			best_res = &res;
		}
	}

	// also tell the user that we're done :)
	print_progress(1, 1);

	return best_res->best_move;
}

// to initialize this as init-time, use a lambda
static int pieces_values[8] = { 0 };
int i = []() {
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

int FossileChess::evaluate_board(Board* board) { // evaluates from whites perspective
	//TODO: better evaluation (e.g. points for rook on open file etc)
	int eval = 0;

	if (board->gameState == GameState::draw) {
		eval = 0;
		return eval;
	} else if (board->gameState == GameState::white_checkmate) {
		eval = -CHECKMATE;
		return eval;
	} else if (board->gameState == GameState::black_checkmate) {
		eval = CHECKMATE;
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

int FossileChess::minimax(Board* board, int depth, int alpha, int beta, bool maximizing_player) {
	if (depth == 0 || board->gameState != GameState::playing) {
		return evaluate_board(board);
	}

	if (maximizing_player) {
		int max_eval = -HIGHEST_VALUE;
		for (Move m : board->all_possible_moves) {
			Board b = *board;
			b.move(m);
			int eval = minimax(&b, depth - 1, alpha, beta, false);
			max_eval = std::max(max_eval, eval);
			alpha = std::max(alpha, eval);
			if (beta <= alpha) {
				break;
			}
		}
		return max_eval;
	} else {
		int min_eval = HIGHEST_VALUE;
		for (Move m : board->all_possible_moves) {
			Board b = *board;
			b.move(m);
			int eval = minimax(&b, depth - 1, alpha, beta, true);
			min_eval = std::min(min_eval, eval);
			beta = std::min(beta, eval);
			if (beta <= alpha) {
				break;
			}
		}
		return min_eval;
	}
}
