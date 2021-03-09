#pragma once

#include <cstdint>

class Piece {
public:
	uint8_t type : 7;
	uint8_t is_white : 1;

	Piece(uint8_t _type, uint8_t _is_white);

	template<typename A, typename B>
	Piece(A _type, B _is_white) : Piece((uint8_t)_type, (uint8_t)_is_white) {}
};