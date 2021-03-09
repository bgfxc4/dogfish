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

//	// who moves next
//	while (1) {
//		if (!*++c)
//			goto err;
//		if (*c == 'w')
//			toMove = Color::White;
//		else if (*c == 'b')
//			toMove = Color::Black;
//		else if (*c == ' ')
//			break;
//		else {
//			std::cout << "[ERROR] invalid fen string at 2nd group" << std::endl;
//			return -1;
//		}
//	}
//
//	// who can castle
//	while (1) {
//		if (!*++c)
//			goto err;
//		if (*c == 'Q')
//			whiteCanCastleLong = true;
//		else if (*c == 'K')
//			whiteCanCastleShot = true;
//		else if (*c == 'q')
//			blackCanCastleLong = true;
//		else if (*c == 'k')
//			blackCanCastleShot = true;
//		else if (*c == ' ')
//			break;
//		else {
//			std::cout << "[ERROR] invalid fen string at 3rd group" << std::endl;
//			return -1;
//		}
//	}
//
//	// possible en passant positions
//	while (1) {
//		if (!*++c)
//			goto err;
//		if (*c == '-')
//			continue;
//		else if (*c == ' ')
//			break;
//		else if (!std::isdigit(*c))
//			temp_char = *c;
//		else if (std::isdigit(*c)) {
//			if (temp_char == 0) {
//				std::cout << "[ERROR] invalid fen string at 4th group(1)" << std::endl;
//				return -1;
//			} else {
//				std::string tileName = std::string(1, temp_char) + *c;
//				int* enPassantTile = tileNameToPosition(tileName);
//				tiles[enPassantTile[0]][enPassantTile[1]]->enPassantPossible = true;
//				temp_char = 0;
//			}
//		} else {
//			std::cout << "[ERROR] invalid fen string at 4th group(2)" << std::endl;
//			return -1;
//		}
//	}
//
//	// half-move counter
//	halfMoves = 0;
//	while (1) {
//		if (!*++c)
//			goto err;
//
//		if (std::isdigit(*c))
//			halfMoves = halfMoves * 10 + (*c - '0');
//		else if (*c == ' ')
//			break;
//		else {
//			std::cout << "[ERROR] invalid fen string at 5th group" << std::endl;
//			return -1;
//		}
//	}
//
//	// move counter
//	moveCount = 0;
//	while (1) {
//		if (!*++c || !(*c-' ')) // mega kek
//			break;
//		else if (std::isdigit(*c))
//			moveCount = moveCount * 10 + (*c - '0');
//		else {
//			std::cout << "[ERROR] invalid fen string at 6th group" << std::endl;
//			return -1;
//		}
//	}

	return 0;

err:
	std::cout << "[ERROR] fen string too short" << std::endl;
	return -1;
}
