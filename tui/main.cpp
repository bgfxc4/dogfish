#include <chrono>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>

#include "board.hpp"
#include "parse_opts.hpp"
#include "dogfish.hpp"

Dogfish engine;
Move* engine_move = nullptr;
std::thread engine_thread;

char piece_to_char(Piece p) {
	char c = ' ';
	switch (p.type) {
		case (int)Pieces::Pawn:
			if (p.is_white)
				c = 'P';
			else 
				c = 'p';
			break;	
		case (int)Pieces::Bishop:
			if (p.is_white)
				c = 'B';
			else 
				c = 'b';
			break;
		case (int)Pieces::Knight:
			if (p.is_white)
				c = 'N';
			else 
				c = 'n';
			break;
		case (int)Pieces::Rook:
			if (p.is_white)
				c = 'R';
			else 
				c = 'r';
			break;	
		case (int)Pieces::Queen:
			if (p.is_white)
				c = 'Q';
			else 
				c = 'q';
			break;
		case (int)Pieces::King:
			if (p.is_white)
				c = 'K';
			else 
				c = 'k';
			break;
		case (int)Pieces::Empty:
			c = ' ';
	}
	return c;
}

void print_current_board(Board* b) {
	std::cout << " -----------------" << std::endl << "8";
	for (int i = 0; i < 64; i++) {
		char c = piece_to_char(b->getPiece(i % 8, i / 8));
		bool last_of_line = (i + 1) % 8 == 0;
		std::cout << "|" << c;
		if (last_of_line) {
			std::cout << "|" << std::endl << " -----------------" << std::endl;
			if (i != 63) std::cout << (char)((8 - (i / 8)) + 47);
		}
	}
	std::cout << "  A B C D E F G H" << std::endl;
}

int letter_to_x(char c) {
	return tolower(c) - 'a';
}

Move input_to_move(std::string input) {
	Move ret = Move(-1, -1, -1, -1);

	if (input.length() != 4) return ret;
	ret.from_x = letter_to_x(input[0]);
	ret.from_y = 8 - (input[1] - '0');
	ret.to_x = letter_to_x(input[2]);
	ret.to_y = 8 - (input[3] - '0');
	return ret;
}

bool is_move_legal(Board* board, Move move) {
	for (Move m : board->all_possible_moves) {
		if (m == move) return true; 
	}
	return false;
}

void spawn_engine(Dogfish* engine, Board* board, Move** out, int threads_to_use, int engine_depth) {
	Move* out_local = new Move(-1, -1, -1, -1);
	*out_local = engine->get_best_move(board, engine_depth, threads_to_use - 1);
	__atomic_store_n(out, out_local, __ATOMIC_SEQ_CST);
}

void make_engine_move(Board* board, int threads_to_use, int engine_depth) {
	engine_thread = std::thread(spawn_engine, &engine, board, &engine_move, threads_to_use, engine_depth);
	engine_thread.detach();

	Move* atomic_move = nullptr;
	while (atomic_move == nullptr) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		atomic_move = __atomic_load_n(&engine_move, __ATOMIC_SEQ_CST);
	}
	board->move(*atomic_move);
	delete engine_move;
	engine_move = nullptr;
}

void make_human_move(Board* board)  {
	std::string move_str;
	print_current_board(board);
	std::cout << std::endl << (board->white_to_move ? "White" : "Black") << " to move. Please enter your move (eg a1a2 or e2e4):" << std::endl;
	std::cin >> move_str;
	
	Move move = Move(-1, -1, -1, -1);
	move = input_to_move(move_str);
	while (!is_move_legal(board, move)) {
		std::cout << "That move is not legal. Please enter your move (eg a1a2 or e2e4):" << std::endl;
		std::cin >> move_str;
		move = input_to_move(move_str);
	}
	board->move(move);
}

int main(int argc, char** argv) {
	struct opts opt;
	parse_opts(argc, argv, &opt);

	if (opt.help) return 0;

	if (opt.threads == -1) {
		opt.threads = std::thread::hardware_concurrency();
	}

	if (opt.depth == -1) {
		opt.depth = 5;
	}

	if (opt.threads < 1) error("The engine needs at least one thread to work!", 1);
	if (opt.depth < 1) error("The engine can at least search to depth one!", 1);

	Board board;

	while (1) {
		if (opt.engine && !board.white_to_move)
			make_engine_move(&board, opt.threads, opt.depth);
		else
			make_human_move(&board);
	}
}
