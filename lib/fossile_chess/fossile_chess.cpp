#include <vector>
#include <iostream>
#include <unistd.h>
#include <algorithm>

#include "board.hpp"
#include "fossile_chess.hpp"
#include "constants.hpp"

FossileChess::FossileChess() {};

Move FossileChess::get_best_move(Board* board, int depth) {
	int best_move_index = 0;
	int best_move_eval = 999999999;
	for (int i = 0; i < (int)board->all_possible_moves.size(); i++) {
		Board b = *board;
		b.move(board->all_possible_moves[i]);
		int eval = minimax(&b, depth - 1, -999999999, 999999999, true);
		if (eval < best_move_eval) {
			best_move_eval = eval;
			best_move_index = i;
		}
		std::cout << (((float)(i+1) / (float)board->all_possible_moves.size()) * 100) << "%\n";
	}
	return board->all_possible_moves[best_move_index];
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
