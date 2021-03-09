#include <cstdio>

#include "board.hpp"

int main() {
	Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq f3 23 21");

	printf("Board size: %zu\n\n", sizeof(board));

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			Piece p = board.getPiece(x, y);
			printf("% 2hhu ", p.type + 8 * p.is_white);
		}
		puts("");
	}
}
