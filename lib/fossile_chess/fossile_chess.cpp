#include <vector>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <thread>

#include "board.hpp"
#include "fossile_chess.hpp"
#include "constants.hpp"

FossileChess::FossileChess() {};

minimax_thread::minimax_thread(Move m) {
	move = m;
};

void spawn_minimax_thread(FossileChess* engine, Board* board, int depth, minimax_thread** out) {
	Board b = *board;
	b.move((*out)->move);
	minimax_thread* out_local = new minimax_thread((*out)->move);
	std::cout << "depth: " <<  depth << std::endl;
	out_local->eval = engine->minimax(&b, depth, -999999999, 999999999, true);
	out_local->state = 1;
	free(*out);
	std::cout << "ended thread " << out_local->move.from_x << " " << out_local->move.from_y << " to "
			  << out_local->move.to_x << " " << out_local->move.to_y << std::endl;
	__atomic_store_n(out, out_local, __ATOMIC_SEQ_CST);
}

Move FossileChess::get_best_move(Board* board, int depth, int threads_to_use) {
	Move best_move(-1, -1, -1, -1);
	int best_move_eval = 999999999;
	
	for (Move m : board->all_possible_moves) {
		minimax_thread** mmt_ptr = (minimax_thread**)malloc(sizeof(minimax_thread*));
		minimax_thread* mmt = new minimax_thread(m);
		*mmt_ptr = mmt;
		moves_to_be_processed.push_back(mmt_ptr);
	}
	
	bool not_finished = true;
	int threads_used = 0;
	int moves_calculated_count = 0;
	while (not_finished) {
		not_finished = false;
		for (minimax_thread** mmt_ptr : moves_to_be_processed) {
			minimax_thread* mmt_local = __atomic_load_n(mmt_ptr, __ATOMIC_SEQ_CST);
			if (mmt_local->state == 2) continue; // move is calculated, can be skipped
			if (mmt_local->state == 0)  { // a thread is already working on this move
				not_finished = true;
				continue; 
			}
			if (mmt_local->state == -1) { // move needs to be calculated
				not_finished = true;
				if (threads_used >= threads_to_use) continue; // all threads are already used
				(*mmt_ptr)->state = 0;
				std::thread th(spawn_minimax_thread, this, board, depth - 1, mmt_ptr); // start new thread
				th.detach();
				threads_used++;
			}
			if (mmt_local->state == 1) {
				(*mmt_ptr)->state = 2;
				not_finished = true;
				threads_used--;

				if (mmt_local->eval < best_move_eval) {
					best_move = mmt_local->move;
					best_move_eval = mmt_local->eval;
				}
				moves_calculated_count++;
				std::cout << ((float)(moves_calculated_count) / (float)moves_to_be_processed.size()) * 100 << "%" << std::endl;
			}
		}
	}
	moves_to_be_processed.clear();
	return best_move;
}

int FossileChess::evaluate_board(Board* board) { // evaluates from whites perspective
	//TODO: better evaluation (e.g. points for rook on open file etc)
	int eval = 0;

	if (board->gameState == GameState::draw) {
		eval = 0;
		return eval;
	} else if (board->gameState == GameState::white_checkmate) {
		eval = -99999999;
		return eval;
	} else if (board->gameState == GameState::black_checkmate) {
		eval = 99999999;
		return eval;
	}

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece p = board->getPiece(x, y);
			if (p.type == (int)Pieces::Empty) {
				continue;
			} else if (p.type == (int)Pieces::Pawn) {
				(p.is_white) ? eval += 100 : eval -= 100;
				// if the pawn is horizontaliy in the middle, it gets points for being close to the center
				if ((x > 1 && x < 6) && ((p.is_white && y >= 4 && y >= 5) || (!p.is_white && y <= 3 && y >= 2))) { 
					if (x > 2 && x < 5) { // if it is closer to the center, it gets more points
						eval += (p.is_white) ? (4 /*maximum points*/ - (y - 4)) : -(4 - (3 - y));
					} else {
						eval += (p.is_white) ? (2 /*maximum points*/ - (y - 4)) : -(2 - (3 - y));
					}
				}
			} else if (p.type == (int)Pieces::Knight) {
				(p.is_white) ? eval += 300 : eval -= 300;
			} else if (p.type == (int)Pieces::Bishop) {
				(p.is_white) ? eval += 310 : eval -= 310;
			} else if (p.type == (int)Pieces::Rook) {
				(p.is_white) ? eval += 500 : eval -= 500;
			} else if (p.type == (int)Pieces::Queen) {
				(p.is_white) ? eval += 900 : eval -= 900;
			} else if (p.type == (int)Pieces::King) {
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
		int max_eval = -999999999;
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
		int min_eval = 999999999;
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
