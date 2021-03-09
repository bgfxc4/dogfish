#include "board.hpp"

#include <array>
#include <vector>
#include <iostream>
#include <cstring>

#include "constants.hpp"

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
	_en_passant_pos = 0;
	white_to_move = 1;

	white_castle_short = 1;
	white_castle_long = 1;
	black_castle_short = 1;
	black_castle_long = 1;

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
	char temp_char = 0; // declaration can't be after first goto

	const char* c = fenString.c_str() - 1;

	// positions
	int row = 0, col = 0;
	while (1) {
		if (!*++c)
			goto err;
		if (*c == '/') {
			row++;
			col = 0;
			continue;
		} else if (std::isdigit(*c)) {
			col += *c - '0';
			continue;
		}

		if (*c == 'R' || *c == 'r')
			bc.set(col, row, Piece(Pieces::Rook, (*c == 'R')));
		else if (*c == 'N' || *c == 'n')
			bc.set(col, row, Piece(Pieces::Knight, (*c == 'N')));
		else if (*c == 'B' || *c == 'b')
			bc.set(col, row, Piece(Pieces::Bishop, (*c == 'B')));
		else if (*c == 'Q' || *c == 'q')
			bc.set(col, row, Piece(Pieces::Queen, (*c == 'Q')));
		else if (*c == 'K' || *c == 'k')
			bc.set(col, row, Piece(Pieces::King, (*c == 'K')));
		else if (*c == 'P' || *c == 'p')
			bc.set(col, row, Piece(Pieces::Pawn, (*c == 'P')));
		else if (*c == ' ')
			break;
		else {
			std::cout << "[ERROR] invalid fen string at 1st group c: " << c << std::endl;
			return -1;
		}

		col++;
	}

	// who moves next
	while (1) {
		if (!*++c)
			goto err;
		if (*c == 'w')
			white_to_move = 1;
		else if (*c == 'b')
			white_to_move = 0;
		else if (*c == ' ')
			break;
		else {
			std::cout << "[ERROR] invalid fen string at 2nd group" << std::endl;
			return -1;
		}
	}

	// who can castle
	while (1) {
		if (!*++c)
			goto err;
		if (*c == 'Q')
			white_castle_long = true;
		else if (*c == 'K')
			white_castle_short = true;
		else if (*c == 'q')
			white_castle_long = true;
		else if (*c == 'k')
			white_castle_short = true;
		else if (*c == ' ')
			break;
		else {
			std::cout << "[ERROR] invalid fen string at 3rd group" << std::endl;
			return -1;
		}
	}

	// possible en passant positions
	while (1) {
		if (!*++c)
			goto err;
		if (*c == '-') {
			clear_en_passant_pos();
			continue;
		}
		else if (*c == ' ')
			break;
		else if (!std::isdigit(*c))
			temp_char = *c;
		else if (std::isdigit(*c)) {
			int x = temp_char - 'a', y = *c - '0';
			if (x < 0 || x >= 8 || y < 0 || y >= 8) {
				std::cout << "[ERROR] invalid fen string at 4th group(2)" << std::endl;
				return -1;
			}

			set_en_passant_pos(std::make_pair(x, y));
		} else {
			std::cout << "[ERROR] invalid fen string at 4th group(2)" << std::endl;
			return -1;
		}
	}

	// half-move counter
	num_half_moves = 0;
	while (1) {
		if (!*++c)
			goto err;

		if (std::isdigit(*c))
			num_half_moves = num_half_moves * 10 + (*c - '0');
		else if (*c == ' ')
			break;
		else {
			std::cout << "[ERROR] invalid fen string at 5th group" << std::endl;
			return -1;
		}
	}

	// move counter
	num_moves = 0;
	while (1) {
		if (!*++c || *c == ' ') // mega kek
			break;
		else if (std::isdigit(*c))
			num_moves = num_moves * 10 + (*c - '0');
		else {
			std::cout << "[ERROR] invalid fen string at 6th group" << std::endl;
			return -1;
		}
	}

	return 0;

err:
	std::cout << "[ERROR] fen string too short" << std::endl;
	return -1;
}

std::vector<std::pair<int, int>> Board::get_moves(int x, int y) {
	Piece from = bc.get(x, y);
	if (from.type == (uint8_t)Pieces::Empty || white_to_move != from.is_white) {
		// quick return on degenerate cases
		return {};
	}

	std::vector<std::pair<int, int>> raw_moves = get_moves_raw(x, y);

	std::vector<std::pair<int, int>> moves_no_friendly_fire;
	for (std::pair<int, int> move : raw_moves) {
		int to_x = std::get<0>(move), to_y = std::get<1>(move);
		Piece to = bc.get(to_x, to_y);

		if (to.type == (uint8_t)Pieces::Empty || to.is_white != from.is_white) {
			moves_no_friendly_fire.push_back(move);
		}
	}

	std::vector<std::pair<int, int>> res;
	for (std::pair<int, int> move : moves_no_friendly_fire) {
		Board tmp(*this);
		tmp.move_raw(x, y, std::get<0>(move), std::get<1>(move));

		if (!tmp.is_check()) {
			res.push_back(move);
		}
	}

	return res;
}

bool Board::is_check() {
	return false;
}

void Board::move_raw(int from_x, int from_y, int to_x, int to_y) {
	// TODO: castling
	bc.set(to_x, to_y, bc.get(from_x, from_y));
	bc.set(from_x, from_y, Piece(Pieces::Empty, 0));
}

std::vector<std::pair<int, int>> Board::get_moves_raw(int x, int y) {
	Piece p = bc.get(x, y);
	std::vector<std::pair<int, int>> res;
	switch ((Pieces)p.type) {
	case Pieces::Rook:
		for (int _x = x - 1; _x >= 0; _x--) {
			res.push_back(std::make_pair(_x, y));
			if (bc.get(_x, y).type != (uint8_t)Pieces::Empty) break;
		}
		for (int _x = x + 1; _x < 8; _x++) {
			res.push_back(std::make_pair(_x, y));
			if (bc.get(_x, y).type != (uint8_t)Pieces::Empty) break;
		}
		for (int _y = y - 1; _y >= 0; _y--) {
			res.push_back(std::make_pair(x, _y));
			if (bc.get(x, _y).type != (uint8_t)Pieces::Empty) break;
		}
		for (int _y = y + 1; _y < 8; _y++) {
			res.push_back(std::make_pair(x, _y));
			if (bc.get(x, _y).type != (uint8_t)Pieces::Empty) break;
		}
		break;

	default:
		break;
	}

	return res;
}
