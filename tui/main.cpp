#include <iostream>
#include <ostream>
#include <string>

#include "board.hpp"

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

int main() {
	Board board;

	while (1) {
		std::string move_str;
		print_current_board(&board);
		std::cout << std::endl << (board.white_to_move ? "White" : "Black") << " to move. Please enter your move (eg a1a2 or e2e4):" << std::endl;
		std::cin >> move_str;
		
		Move move = Move(-1, -1, -1, -1);
		move = input_to_move(move_str);
		while (!is_move_legal(&board, move)) {
			std::cout << "That move is not legal. Please enter your move (eg a1a2 or e2e4):" << std::endl;
			std::cin >> move_str;
			move = input_to_move(move_str);
		}
		board.move(move);
	}
}
