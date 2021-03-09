#include <cstdio>

#include "board.hpp"

void print_moves(Board& board, int x, int y) {
	printf("%d, %d:\n", x, y);
	auto moves = board.get_moves(x, y);
	for (auto& move : moves) {
		printf("-> %d, %d\n", std::get<0>(move), std::get<1>(move));
	}
	puts("");
}

int main() {
	Board board("rnbqkbnr/pppppppp/8/R3n3/4B3/3b4/PPPPPPPP/RNBQKBNR w KQkq f3 23 21");

	printf("Board size: %zu\n\n", sizeof(board));

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			Piece p = board.getPiece(x, y);
			printf("% 2hhu ", p.type + 8 * p.is_white);
		}
		puts("");
	}

	puts("");
	print_moves(board, 0, 3);
	print_moves(board, 4, 4);
}
