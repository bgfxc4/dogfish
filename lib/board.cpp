#include "board.hpp"

#include <array>
#include <vector>
#include <iostream>

#include "constants.hpp"

Position::Position(int x, int y) {
	this->x = x;
	this->y = y;
}

bool Position::operator== (Position second) {
	return this->x == second.x && this->y == second.y;
}

Move::Move(int to_x, int to_y, int is_promotion) {
	this->to_x = to_x;
	this->to_y = to_y;
	this->is_promotion = is_promotion;
}

bool Move::operator== (Move& second) {
	return this->to_x == second.to_x && this->to_y == second.to_y && this->is_promotion == second.is_promotion;
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

std::optional<Position> Board::get_en_passant_pos() {
	if (!(_en_passant_pos & 0x40)) {
		return {};
	}

	uint8_t tmp = _en_passant_pos & 0x3F;

	return Position(tmp >> 3, tmp & 0x07);
}

void Board::set_en_passant_pos(Position pos) {
	_en_passant_pos = 0x40 | (pos.x << 3) | pos.y;
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

			set_en_passant_pos({x, y});
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

std::vector<Move> Board::get_moves(int x, int y) {
	Piece from = bc.get(x, y);
	if (from.type == (uint8_t)Pieces::Empty || white_to_move != from.is_white) {
		// quick return on degenerate cases
		return {};
	}

	std::vector<Move> raw_moves = get_moves_raw(x, y);

	std::vector<Move> moves_no_friendly_fire;
	for (Move move : raw_moves) {
		int to_x = move.to_x, to_y = move.to_y;
		Piece to = bc.get(to_x, to_y);

		if (to.type == (uint8_t)Pieces::Empty || to.is_white != from.is_white) {
			moves_no_friendly_fire.push_back(move);
		}
	}

	std::vector<Move> res;
	for (Move move : moves_no_friendly_fire) {
		Board tmp(*this);
		tmp.move_raw(x, y, move.to_x, move.to_y);

		if (!tmp.is_check()) {
			res.push_back(move);
		}
	}

	return res;
}

bool Board::is_check() {
	Piece king(0, 0);
	Position kingPos(-1, -1);
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece p = bc.get(x, y);
			if (p.type == (int)Pieces::King && p.is_white == white_to_move) {
				king = p; // find king of the player who has to move
				kingPos = Position(x, y);
			}
		}
	}

	if (tile_is_attacked(!king.is_white, kingPos.x, kingPos.y)) {
		return true;
	}
	return false;
}

bool Board::tile_is_attacked(uint8_t color, int tileX, int tileY) {
	if (tile_is_attacked_straight_diagonal(color, tileX, tileY)) {
		return true;
	}

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece p = bc.get(x, y);
			if (p.type != (int)Pieces::Empty) {
				if (p.is_white != color) continue;
				if (p.type == (int)Pieces::Pawn || p.type == (int)Pieces::Knight || p.type == (int)Pieces::King) {
					std::vector<Move> moves = get_moves_raw(x ,y);
					for (Move move : moves) {
						if ((move.to_x == tileX) && (move.to_y == tileY)) return true;
					}
				}
			}
		}
	}
	return false;
}
// overload to ignore a specific piece
bool Board::tile_is_attacked(uint8_t color, int tileX, int tileY, bool ignoreKings) { 
	if (tile_is_attacked_straight_diagonal(color, tileX, tileY)) {
		return true;
	}

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece p = bc.get(x, y);
			if (p.type != (int)Pieces::Empty) {
				if (p.is_white != color) continue;
				if (p.type == (int)Pieces::Pawn || p.type == (int)Pieces::Knight || 
					(p.type == (int)Pieces::King && !ignoreKings)) 
				{
					std::vector<Move> moves = get_moves_raw(x ,y);
					for (Move move : moves) {
						if ((move.to_x == tileX) && (move.to_y == tileY)) return true;
					}
				}
			}
		}
	}
	return false;
}


bool Board::tile_is_attacked_straight_diagonal(uint8_t color, int tileX, int tileY) {
	
	std::vector<Position> mods = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

	for (Position mod : mods) { // check all diagonals of tile
		int dx = mod.x, dy = mod.y;
		for (int y = tileY + dy, x = tileX + dx;
				y >= 0 && y < 8 && x >= 0 && x < 8;
				y += dy, x += dx)
		{
			Piece p = bc.get(x, y);
			if (p.type != (uint8_t)Pieces::Empty) {
				// first piece on diagonal is from opponent an can hit over diagonals
				if (p.is_white == color && (p.type == (int)Pieces::Bishop || p.type == (int)Pieces::Queen)) {
					return true;
				}
				break;
			}
		}
	}

	mods = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

	for (Position mod : mods) { // check file and row of king
		int dx = mod.x, dy = mod.y;
		for (int y = tileY + dy, x = tileX + dx;
				y >= 0 && y < 8 && x >= 0 && x < 8;
				y += dy, x += dx)
		{
			Piece p = bc.get(x, y);
			if (p.type != (uint8_t)Pieces::Empty) {
				// piece on file or row is from opponent an can hit over 
				if (p.is_white == color && (p.type == (int)Pieces::Rook || p.type == (int)Pieces::Queen)) {
					return true;
				}
				break;
			}
		}
	}

	return false;
}

void Board::move_raw(int from_x, int from_y, int to_x, int to_y) {
	// TODO: castling
	bc.set(to_x, to_y, bc.get(from_x, from_y));
	bc.set(from_x, from_y, Piece(Pieces::Empty, 0));
}

void Board::move(int from_x, int from_y, int to_x, int to_y) {
	Piece toMove = bc.get(from_x, from_y);
	std::vector<Move> possibleMoves = get_moves(from_x, from_y);
	bool legalMove = false;
	for (Move move : possibleMoves) {
		if (move.to_x == to_x && move.to_y == to_y) {
			legalMove = true;
			break;
		}
	}
	if (!legalMove) return;

	Position tmp_en_passant_pos = get_en_passant_pos().value_or((Position(-1, -1)));
	clear_en_passant_pos();

	if (toMove.type == (int)Pieces::Rook) { // you cant castle anymore if you move a rook
		if (from_y == 7 && white_to_move == 1) {
			if (from_x == 0)
				white_castle_long = false;
			else if (from_x == 7)
				white_castle_short = false;
		} else if (from_y == 0 && white_to_move == 0) {
			if (from_x == 0)
				black_castle_long = false;
			else if (from_x == 7)
				black_castle_short = false;
		}
	} else if (toMove.type == (int)Pieces::King) { 

		if (from_x == 4 && from_y == 7 && white_to_move == 1) { // player tried to castle
			if (to_x == 2 && to_y == 7 && white_castle_long) {
				move_raw(0, 7, 3, 7);
			} else if (to_x == 6 && to_y == 7 && white_castle_short) {
				move_raw(7, 7, 5, 7);
			}
		} else if (from_x == 4 && from_y == 0 && white_to_move == 0) {
			if (to_x == 2 && to_y == 0 && black_castle_long) {
				move_raw(0, 0, 3, 0);
			} else if (to_x == 6 && to_y == 0 && black_castle_short) {
				move_raw(7, 0, 5, 0);
			}
		}

		if (white_to_move == 1) { // you cant castle anymore if you move your king
			white_castle_short = false;
			white_castle_long = false;
		} else {
			black_castle_short = false;
			black_castle_long = false;
		}
	} 

	if (toMove.type == (int)Pieces::Pawn) {
		int mod = (white_to_move == 1) ? -1 : 1;
		if (to_y - from_y == 2 * mod) { // pawn was moved two tiles forward, en passant possible on next move
			set_en_passant_pos({from_x, from_y + mod});
		} else if (tmp_en_passant_pos == Position(to_x, to_y)) { // en passant happened
			bc.clear_tile(to_x, to_y + (-1 * mod));
		}
	}
	
	move_raw(from_x, from_y, to_x, to_y);
	white_to_move = !white_to_move;
}

std::vector<Move> Board::get_moves_raw(int x, int y) {
	return bc.get(x, y).get_moves_raw(*this, x, y);
}
