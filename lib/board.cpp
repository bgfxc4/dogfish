#include <array>
#include <vector>
#include <iostream>
#include <cstring>

#include "board.hpp"

BoardContent::BoardContent() {
	memset(bits, 0, sizeof(bits));
}

Piece BoardContent::get(int x, int y) {
	return get(y * 8 + x);
}

void BoardContent::set(int x, int y, Piece p) {
	set(y * 8 + x, p.type | (p.is_white << 3));
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

Board::Board(const std::string& fenString) {
	std::cout << "asd" << std::endl;
	_en_passant_pos = 0;
	white_to_move = 1;
	parseFenString(fenString);
}

std::optional<std::pair<int, int>> Board::get_en_passant_pos() {
	if (!(_en_passant_pos & 0x40)) {
		return {};
	}

	uint8_t tmp = _en_passant_pos & 0x3F;

	return std::make_pair<int, int>(tmp >> 3, tmp & 0x07);
}

void Board::set_en_passant_pos(std::pair<int, int> pos) {
	_en_passant_pos = 0x40 | (std::get<0>(pos) << 3) | (std::get<1>(pos));
}

void Board::clear_en_passant_pos() {
	_en_passant_pos = 0;
}

Piece Board::getPiece(int x, int y) {
	return bc.get(x, y);
}

int Board::parseFenString(const std::string& fenString) {
	std::cout << "Would parse: " << fenString.c_str() << std::endl;
}
