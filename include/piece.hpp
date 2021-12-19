#pragma once

#include <cstdint>
#include <vector>

class Move;
class Board;
class Position;

class Piece {
public:
	uint8_t type : 7;
	uint8_t is_white : 1;

	// is_white must be 0 or 1
	Piece(uint8_t _type, uint8_t _is_white) : type(_type), is_white(_is_white) {}

	template<typename A, typename B>
	Piece(A _type, B _is_white) : Piece((uint8_t)_type, (uint8_t)_is_white) {}

	void get_moves_raw(Board& board, int x, int y, std::vector<Move>& res);
	uint64_t get_attacked_tiles(Board& board, int x, int y);
	static uint64_t get_king_attackers(Board& board, bool is_white);
};
