#include "board.hpp"

#include <cstring>

BoardContent::BoardContent() {
	memset(bits, 0, sizeof(bits));
}

Piece BoardContent::get(int x, int y) {
	return get(y * 8 + x);
}

uint8_t* BoardContent::get_all_raw() {
	return bits;
}

void BoardContent::set(int x, int y, Piece p) {
	set(y * 8 + x, p.type | (p.is_white << 3));
}

void BoardContent::clear_tile(int x, int y) {
	set(y * 8 + x, 0 | (1 << 3));
}

Piece BoardContent::get(uint8_t n) {
	uint8_t byte = bits[n >> 1];
	if (n & 1) {
		byte >>= 4;
	}
	else {
		byte &= 0x0F;
	}
	return { (uint8_t)(byte & 0x07), (uint8_t)(byte >> 3) };
}

void BoardContent::set(uint8_t n, uint8_t p) {
	if (n & 1) {
		bits[n >> 1] &= 0x0F;
		bits[n >> 1] |= p << 4;
	}
	else {
		bits[n >> 1] &= 0xF0;
		bits[n >> 1] |= p;
	}
}
