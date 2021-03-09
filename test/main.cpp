#include <cstdio>

#include "board.hpp"

int main() {
	Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	Piece p = board.getPiece(0, 0);
	printf("%hhu, %hhu\n", p.type, p.is_white);
}
