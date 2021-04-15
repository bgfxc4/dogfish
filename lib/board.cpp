#include <array>
#include <bits/stdint-uintn.h>
#include <vector>
#include <iostream>
#include <cstring>

#include "constants.hpp"
#include "board.hpp"

bool Position::operator== (Position second) {
	return this->x == second.x && this->y == second.y;
}

bool Position::operator!= (Position second) {
	return this->x != second.x || this->y != second.y;
}

bool Move::operator== (Move& second) {
	return this->to_x == second.to_x && this->to_y == second.to_y && this->promote_to == second.promote_to;
}

BoardLite::BoardLite(Board* board) {
	this->bc = board->bc;
	this->white_to_move = board->white_to_move;
	this->white_castle_short = board->white_castle_short;
	this->white_castle_long = board->white_castle_long;
	this->black_castle_short = board->black_castle_short;
	this->black_castle_long = board->black_castle_long;
	this->_en_passant_pos = board->_en_passant_pos;
}

uint8_t* BoardLite::get_all_raw() {
	return bc.get_all_raw();
}

Board::Board(const std::string& fenString) {
	_en_passant_pos = 0;
	white_to_move = 1;

	white_castle_short = 1;
	white_castle_long = 1;
	black_castle_short = 1;
	black_castle_long = 1;

	parseFenString(fenString);
	find_kings();
	calculate_all_attacked_tiles();
	calculate_all_possible_moves();
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
	std::vector<Move> res;

	for (Move m : all_possible_moves) {
		if (m.from_x == x && m.from_y == y) res.push_back(m);
	}

	return res;
}

bool Board::is_check() {
	Position king_pos(-1, -1);
	(white_to_move) ? king_pos = white_king : king_pos = black_king;
	return tile_is_attacked(king_pos.x, king_pos.y);
}

bool Board::is_check_slow() {
	Position king_pos(-1, -1);
	(white_to_move) ? king_pos = white_king : king_pos = black_king;

	std::vector<Position> mods = { Position(1, 1), Position(1, -1), Position(-1, 1), Position(-1, -1),
									Position(0, 1), Position(1, 0), Position(0, -1), Position(-1, 0)};
	
	for (Position mod : mods) {
		int dx = mod.x, dy = mod.y;
		for (int _y = king_pos.y + dy, _x = king_pos.x + dx;
				_y >= 0 && _y < 8 && _x >= 0 && _x < 8;
				_y += dy, _x += dx)
		{
			Piece p = getPiece(_x, _y);
			if (p.type == (int)Pieces::Empty) continue;
			if (p.is_white == white_to_move) {
				break;
			} else {
				if (dx * dy == 0 && (p.type == (int)Pieces::Rook || p.type == (int)Pieces::Queen)) {
					return true;
				} else if (dx * dy != 0 && (p.type == (int)Pieces::Bishop || p.type == (int)Pieces::Queen)) {
					return true;
				} else {
					break;
				}
			}
		}
	}
	return false;
}

bool Board::is_insufficient_material() {
	int white_bishop_count = 0;
	int black_bishop_count = 0;
	int white_knight_count = 0;
	int black_knight_count = 0;
	
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece p = bc.get(x, y);
			if (p.type == (int)Pieces::Empty || p.type == (int)Pieces::King) continue;
			else if (p.type == (int)Pieces::Bishop) (p.is_white) ? white_bishop_count++ : black_bishop_count++;
			else if (p.type == (int)Pieces::Knight) (p.is_white) ? white_knight_count++ : black_knight_count++;
			else if (p.type == (int)Pieces::Pawn) return false;
			else if (p.type == (int)Pieces::Rook) return false;
			else if (p.type == (int)Pieces::Queen) return false;
		}
	}
	if (white_knight_count + white_bishop_count <= 1 && black_knight_count + black_bishop_count <= 1) return true;
	return false;
}

uint8_t* Board::get_all_raw() {
	return bc.get_all_raw();
}

void Board::add_position_to_whole_game() {
	BoardLite tmp = BoardLite(this);
	whole_game.push_back(tmp);
}

void Board::find_kings() {
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece p = bc.get(x, y);
			if (p.type == (int)Pieces::King) {
				if (p.is_white) {
					white_king.x = x;
					white_king.y = y;
				} else {
					black_king.x = x;
					black_king.y = y;
				}
			}
		}
	}
}

void Board::calculate_all_attacked_tiles() {
	calculate_pinned_pieces();
	memset(attacked_tiles, 0, 8);
	memset(attacked_tiles_ign_king, 0, 8);
	std::vector<Position> tiles;
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece p = bc.get(x, y);
			if (p.type == (int)Pieces::Empty || (p.is_white == white_to_move)) continue;
			p.get_attacked_tiles(*this, x, y, tiles);
			for (Position pos : tiles) {
				int i = pos.x * 8 + pos.y;
				attacked_tiles[i / 8] |= 1 << (i % 8);
				if (p.type != (int)Pieces::King) attacked_tiles_ign_king[i / 8] |= 1 << (i % 8);
			}
		}
	}
}

void Board::calculate_pinned_pieces() {
	Position king_pos(-1, -1);
	(white_to_move) ? king_pos = white_king : king_pos = black_king;
	pinned_pieces.clear();
	std::vector<Position> mods = { Position(1, 1), Position(1, -1), Position(-1, 1), Position(-1, -1),
									Position(0, 1), Position(1, 0), Position(0, -1), Position(-1, 0)};
	
	for (Position mod : mods) {
		int dx = mod.x, dy = mod.y;
		int friendly_pieces_count = 0;
		
		Position potentially_pinned_piece(-1, -1);

		for (int _y = king_pos.y + dy, _x = king_pos.x + dx;
				_y >= 0 && _y < 8 && _x >= 0 && _x < 8;
				_y += dy, _x += dx)
		{
			Piece p = getPiece(_x, _y);
			if (p.type == (int)Pieces::Empty) continue;
			if (p.is_white == white_to_move) {
				if (friendly_pieces_count == 0) {
					friendly_pieces_count++;
					potentially_pinned_piece.x = _x;
					potentially_pinned_piece.y = _y;
				}
				else break;
			} else {
				if (dx * dy == 0 && (p.type == (int)Pieces::Rook || p.type == (int)Pieces::Queen)) {
					if (friendly_pieces_count == 1) pinned_pieces.push_back(potentially_pinned_piece);
					break;
				} else if (dx * dy != 0 && (p.type == (int)Pieces::Bishop || p.type == (int)Pieces::Queen)) {
					if (friendly_pieces_count == 1) pinned_pieces.push_back(potentially_pinned_piece);
					break;
				} else {
					break;
				}
			}
		}
	}
}

void Board::calculate_all_possible_moves() {
	all_possible_moves.clear();

	// the average branching factor is about 31, so reserve a little more than that
	// to make reallocations unlikely
	std::vector<Move> raw_moves;
	raw_moves.reserve(64);

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece from = bc.get(x, y);
			
			if (from.type == (uint8_t)Pieces::Empty || white_to_move != from.is_white) {
				// quick return on degenerate cases
				continue;
			}
			get_moves_raw(x, y, raw_moves);

			bool is_pinned_piece = false;
				
			for (Position p : pinned_pieces) {
				if (x == p.x && y == p.y) {
					is_pinned_piece = true;
					break;
				}
			}

			for (Move move : raw_moves) {
				// skip moves that would be friendly fire
				int to_x = move.to_x, to_y = move.to_y;
				Piece to = bc.get(to_x, to_y);
				if (to.type != (uint8_t)Pieces::Empty && to.is_white == from.is_white)
					continue;

				if (from.type == (int)Pieces::King) {
					if (tile_is_attacked(move.to_x, move.to_y)) continue;
					Board tmp = *this;
					tmp.move_raw(move.from_x, move.from_y, move.to_x, move.to_y);
					tmp.calculate_all_attacked_tiles();
					if (tmp.is_check()) continue;
					all_possible_moves.push_back(move);
					continue;
				}

				if (is_check() || is_pinned_piece) {
					Board tmp = *this;
					tmp.move_raw(move.from_x, move.from_y, move.to_x, move.to_y);
					tmp.calculate_all_attacked_tiles();
					if (tmp.is_check()) continue;
					all_possible_moves.push_back(move);
					continue; 
				} 
				all_possible_moves.push_back(move);
			}
		}
	}
}

bool Board::tile_is_attacked(int x, int y) {
	int i = x * 8 + y;
	return (attacked_tiles[i / 8] & (1 << (i % 8))) != 0;
}
// overload to ignore a specific piece
bool Board::tile_is_attacked(int x, int y, bool ignoreKings) { 
	int i = x * 8 + y;
	if (ignoreKings)
		return (attacked_tiles_ign_king[i / 8] & (1 << (i % 8))) != 0;
	else 
		return (attacked_tiles[i / 8] & (1 << (i % 8))) != 0;
}

bool Board::is_same_position(BoardLite& board) {
	uint8_t* own_pos = this->get_all_raw();
	uint8_t* compare_pos = board.get_all_raw();
	for (int i = 0; i < 32; i++) {
		if (own_pos[i] != compare_pos[i]) return false;
	}
	if (this->white_castle_short != board.white_castle_short) return false;
	if (this->white_castle_long != board.white_castle_long) return false;
	if (this->black_castle_long != board.black_castle_long) return false;
	if (this->black_castle_short != board.black_castle_short) return false;
	if (this->_en_passant_pos != board._en_passant_pos) return false;
	return true;
}

void Board::move_raw(int from_x, int from_y, int to_x, int to_y) {
	Piece p = bc.get(from_x, from_y);
	if (p.type == (int)Pieces::King) {
		if (p.is_white) {
			white_king.x = to_x;
			white_king.y = to_y;
		} else {
			black_king.x = to_x;
			black_king.y = to_y;
		}
	}
		
	bc.set(to_x, to_y, bc.get(from_x, from_y));
	bc.set(from_x, from_y, Piece(Pieces::Empty, 0));
}

void Board::move(Move move) {
	Piece toMove = bc.get(move.from_x, move.from_y);
	std::vector<Move> possibleMoves = get_moves(move.from_x, move.from_y);
	bool legalMove = false;
	for (Move possibleMove : possibleMoves) {
		if (possibleMove.to_x == move.to_x && possibleMove.to_y == move.to_y) {
			legalMove = true;
			break;
		}
	}
	if (!legalMove) return;

	Position tmp_en_passant_pos = get_en_passant_pos().value_or((Position(-1, -1)));
	clear_en_passant_pos();
	
	if (bc.get(move.to_x, move.to_y).type != (int)Pieces::Empty) { // capture happened
		whole_game.clear();
	}

	if (toMove.type == (int)Pieces::Rook) { // you cant castle anymore if you move a rook
		if (move.from_y == 7 && white_to_move == 1) {
			if (move.from_x == 0)
				white_castle_long = false;
			else if (move.from_x == 7)
				white_castle_short = false;
		} else if (move.from_y == 0 && white_to_move == 0) {
			if (move.from_x == 0)
				black_castle_long = false;
			else if (move.from_x == 7)
				black_castle_short = false;
		}
	} else if (toMove.type == (int)Pieces::King) { 
		if (toMove.is_white) {
			white_king.x = move.to_x;
			white_king.y = move.to_y;
		} else {
			black_king.x = move.to_x;
			black_king.y = move.to_y;
		}
		if (move.from_x == 4 && move.from_y == 7 && white_to_move == 1) { // player tried to castle
			if (move.to_x == 2 && move.to_y == 7 && white_castle_long) {
				move_raw(0, 7, 3, 7);
			} else if (move.to_x == 6 && move.to_y == 7 && white_castle_short) {
				move_raw(7, 7, 5, 7);
			}
		} else if (move.from_x == 4 && move.from_y == 0 && white_to_move == 0) {
			if (move.to_x == 2 && move.to_y == 0 && black_castle_long) {
				move_raw(0, 0, 3, 0);
			} else if (move.to_x == 6 && move.to_y == 0 && black_castle_short) {
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
		if (move.to_y - move.from_y == 2 * mod) { // pawn was moved two tiles forward, en passant possible on next move
			set_en_passant_pos({move.from_x, move.from_y + mod});
		} else if (tmp_en_passant_pos == Position(move.to_x, move.to_y)) { // en passant happened
			bc.clear_tile(move.to_x, move.to_y + (-1 * mod));
		}
	}
	
	move_raw(move.from_x, move.from_y, move.to_x, move.to_y);

	if (move.promote_to != Pieces::Empty) {
		bc.set(move.to_x, move.to_y, Piece(move.promote_to, move.promotion_is_white));
	}

	white_to_move = !white_to_move;
	
	if (toMove.type == (int)Pieces::Pawn || getPiece(move.to_x, move.to_y).type != (int)Pieces::Empty) {
		num_half_moves = 0;
	} else {
		num_half_moves++;
		if (num_half_moves >= 50) {
			gameState = GameState::draw;
		}
	}
	
	int repetition_count = 0;
	for (BoardLite b : whole_game) {
		if (is_same_position(b)) repetition_count++;
	}
	
	if (repetition_count >= 2) {
		gameState = GameState::draw;
	}
	
	if (is_insufficient_material()) {	
		gameState = GameState::draw;
	}
	
	calculate_all_attacked_tiles();
	calculate_all_possible_moves();

	if (all_possible_moves.size() == 0) {
		if (is_check()) {
			gameState = (white_to_move) ? GameState::white_checkmate : GameState::black_checkmate;
		} else {
			gameState = GameState::draw;
		}
	}

	add_position_to_whole_game();
}

void Board::get_moves_raw(int x, int y, std::vector<Move>& res) {
	return bc.get(x, y).get_moves_raw(*this, x, y, res);
}
