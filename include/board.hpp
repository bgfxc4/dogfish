#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>

#include "piece.hpp"

class BoardContent {
public:
	BoardContent();
	Piece get(int x, int y);
	void set(int x, int y, Piece p);

private:
	uint8_t bits[32];
	Piece get(uint8_t n);
	void set(uint8_t n, uint8_t p);
};

class Board {
public:
	Board(const std::string& fenString);
	std::optional<std::pair<int, int>> get_en_passant_pos();
	void set_en_passant_pos(std::pair<int, int> pos);
	void clear_en_passant_pos();
	Piece getPiece(int x, int y);

private:
	BoardContent bc;
	uint8_t _en_passant_pos : 7;
	uint8_t white_to_move : 1;
	int parseFenString(const std::string& fenString);
};
