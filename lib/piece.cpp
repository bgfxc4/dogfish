#include <iostream>
#include <cstdlib>

#include "piece.hpp"
#include "board.hpp"
#include "constants.hpp"

void add_rook_moves(Board& board, int x, int y, std::vector<Move>& res) {
	for (int _x = x - 1; _x >= 0; _x--) {
		res.push_back(Move(x, y, _x, y));
		if (board.getPiece(_x, y).type != (uint8_t)Pieces::Empty) break;
	}
	for (int _x = x + 1; _x < 8; _x++) {
		res.push_back(Move(x, y, _x, y));
		if (board.getPiece(_x, y).type != (uint8_t)Pieces::Empty) break;
	}
	for (int _y = y - 1; _y >= 0; _y--) {
		res.push_back(Move(x, y, x, _y));
		if (board.getPiece(x, _y).type != (uint8_t)Pieces::Empty) break;
	}
	for (int _y = y + 1; _y < 8; _y++) {
		res.push_back(Move(x, y, x, _y));
		if (board.getPiece(x, _y).type != (uint8_t)Pieces::Empty) break;
	}
}

template<bool only_specific_piece = false, Pieces to_check_for = Pieces::Empty>
uint64_t get_rook_attacked_tiles(Board& board, int x, int y, bool is_white, int ignore_king) {
	uint64_t ret = 0;
	for (int _x = x - 1; _x >= 0; _x--) {
		if (!only_specific_piece || (board.getPiece(_x, y).type == (uint8_t)to_check_for
				&& board.getPiece(_x, y).is_white != is_white)) {
			int i = _x * 8 + y;
			ret |= (uint64_t)1 << i;
		}
		Piece p = board.getPiece(_x, y);
		if (ignore_king == 0) {
			if (p.type != (uint8_t)Pieces::Empty && board.white_king.x != _x && board.white_king.y != y) {
				break;
			}
		} else if (ignore_king == 1) {
			if (p.type != (uint8_t)Pieces::Empty && board.black_king.x != _x && board.black_king.y != y) {
				break;
			}
		} else {
			if (p.type != (uint8_t)Pieces::Empty) {
				break;
			}
		}
	}
	for (int _x = x + 1; _x < 8; _x++) {
		if (!only_specific_piece || (board.getPiece(_x, y).type == (uint8_t)to_check_for
				&& board.getPiece(_x, y).is_white != is_white)) {
			int i = _x * 8 + y;
			ret |= (uint64_t)1 << i;
		}
		Piece p = board.getPiece(_x, y);
		if (ignore_king == 0) {
			if (p.type != (uint8_t)Pieces::Empty && board.white_king.x != _x && board.white_king.y != y) {
				break;
			}
		} else if (ignore_king == 1) {
			if (p.type != (uint8_t)Pieces::Empty && board.black_king.x != _x && board.black_king.y != y) {
				break;
			}
		} else {
			if (p.type != (uint8_t)Pieces::Empty) {
				break;
			}
		}
	}
	for (int _y = y - 1; _y >= 0; _y--) {
		if (!only_specific_piece || (board.getPiece(x, _y).type == (uint8_t)to_check_for
				&& board.getPiece(x, _y).is_white != is_white)) {
			int i = x * 8 + _y;
			ret |= (uint64_t)1 << i;
		}
		Piece p = board.getPiece(x, _y);
		if (ignore_king == 0) {
			if (p.type != (uint8_t)Pieces::Empty && board.white_king.x != x && board.white_king.y != _y) {
				break;
			}
		} else if (ignore_king == 1) {
			if (p.type != (uint8_t)Pieces::Empty && board.black_king.x != x && board.black_king.y != _y) {
				break;
			}
		} else {
			if (p.type != (uint8_t)Pieces::Empty) {
				break;
			}
		}
	}
	for (int _y = y + 1; _y < 8; _y++) {
		if (!only_specific_piece || (board.getPiece(x, _y).type == (uint8_t)to_check_for
				&& board.getPiece(x, _y).is_white != is_white)) {
			int i = x * 8 + _y;
			ret |= (uint64_t)1 << i;
		}
		Piece p = board.getPiece(x, _y);
		if (ignore_king == 0) {
			if (p.type != (uint8_t)Pieces::Empty && board.white_king.x != x && board.white_king.y != _y) {
				break;
			}
		} else if (ignore_king == 1) {
			if (p.type != (uint8_t)Pieces::Empty && board.black_king.x != x && board.black_king.y != _y) {
				break;
			}
		} else {
			if (p.type != (uint8_t)Pieces::Empty) {
				break;
			}
		}
	}
	return ret;
}
template uint64_t get_rook_attacked_tiles<false, Pieces::Empty>(Board&, int, int, bool, int);
template uint64_t get_rook_attacked_tiles<false, Pieces::Pawn>(Board&, int, int, bool, int);
template uint64_t get_rook_attacked_tiles<false, Pieces::Knight>(Board&, int, int, bool, int);
template uint64_t get_rook_attacked_tiles<false, Pieces::Bishop>(Board&, int, int, bool, int);
template uint64_t get_rook_attacked_tiles<false, Pieces::Rook>(Board&, int, int, bool, int);
template uint64_t get_rook_attacked_tiles<false, Pieces::Queen>(Board&, int, int, bool, int);
template uint64_t get_rook_attacked_tiles<false, Pieces::King>(Board&, int, int, bool, int);

uint64_t get_rook_attacked_tiles(Board& board, int x, int y) {
	return get_rook_attacked_tiles(board, x, y, board.getPiece(x, y).is_white, -1);
}

uint64_t get_rook_attacked_tiles(Board& board, int x, int y, int ignore_king) {
	return get_rook_attacked_tiles(board, x, y, board.getPiece(x, y).is_white, ignore_king);
}

void add_bishop_moves(Board& board, int x, int y, std::vector<Move>& res) {
	std::array<Position, 4> mods({ Position(-1, -1), Position(-1, 1), Position(1, -1), Position(1, 1)});

	for (Position mod : mods) {
		int dx = mod.x, dy = mod.y;
		for (int _y = y + dy, _x = x + dx;
				_y >= 0 && _y < 8 && _x >= 0 && _x < 8;
				_y += dy, _x += dx)
		{
			res.push_back(Move(x, y, _x, _y));
			if (board.getPiece(_x, _y).type != (uint8_t)Pieces::Empty) break;
		}
	}
}

template<bool only_specific_piece = false, Pieces to_check_for = Pieces::Empty>
uint64_t get_bishop_attacked_tiles(Board& board, int x, int y, bool is_white, int ignore_king) {
	std::array<Position, 4> mods({ Position(-1, -1), Position(-1, 1), Position(1, -1), Position(1, 1)});
	uint64_t ret = 0;

	for (Position mod : mods) {
		int dx = mod.x, dy = mod.y;
		for (int _y = y + dy, _x = x + dx;
				_y >= 0 && _y < 8 && _x >= 0 && _x < 8;
				_y += dy, _x += dx)
		{
			if (!only_specific_piece || (board.getPiece(_x, _y).type == (uint8_t)to_check_for
					&& board.getPiece(_x, _y).is_white != is_white)) {
				int i = _x * 8 + _y;
				ret |= (uint64_t)1 << i;
			}
			Piece p = board.getPiece(_x, _y);
			if (ignore_king == 0) {
				if (p.type != (uint8_t)Pieces::Empty && board.white_king.x != _x && board.white_king.y != _y) {
					break;
				}
			} else if (ignore_king == 1) {
				if (p.type != (uint8_t)Pieces::Empty && board.black_king.x != _x && board.black_king.y != _y) {
					break;
				}
			} else {
				if (p.type != (uint8_t)Pieces::Empty) {
					break;
				}
			}
		}
	}
	return ret;
}
template uint64_t get_bishop_attacked_tiles<false, Pieces::Empty>(Board&, int, int, bool, int);
template uint64_t get_bishop_attacked_tiles<false, Pieces::Pawn>(Board&, int, int, bool, int);
template uint64_t get_bishop_attacked_tiles<false, Pieces::Knight>(Board&, int, int, bool, int);
template uint64_t get_bishop_attacked_tiles<false, Pieces::Bishop>(Board&, int, int, bool, int);
template uint64_t get_bishop_attacked_tiles<false, Pieces::Rook>(Board&, int, int, bool, int);
template uint64_t get_bishop_attacked_tiles<false, Pieces::Queen>(Board&, int, int, bool, int);
template uint64_t get_bishop_attacked_tiles<false, Pieces::King>(Board&, int, int, bool, int);

uint64_t get_bishop_attacked_tiles(Board& board, int x, int y) {
	return get_bishop_attacked_tiles(board, x, y, board.getPiece(x, y).is_white, -1);
}

uint64_t get_bishop_attacked_tiles(Board& board, int x, int y, int ignore_king) {
	return get_bishop_attacked_tiles(board, x, y, board.getPiece(x, y).is_white, ignore_king);
}

void add_knight_moves(int x, int y, std::vector<Move>& res) {
	std::array<Position, 8> moves({ Position(2, 1), Position(2, -1), Position(-2, 1), Position(-2, -1), 
												Position(1, 2), Position(1, -2), Position(-1, 2), Position(-1, -2)});
	for (Position move : moves) {
		if (x + move.x > 7 || x + move.x < 0 || y + move.y > 7 || y + move.y < 0) continue;
		res.push_back(Move(x, y, x + move.x, y + move.y));
	}
}

template<bool only_specific_piece = false, Pieces to_check_for = Pieces::Empty>
uint64_t get_knight_attacked_tiles(Board& board, int x, int y, bool is_white) {
	uint64_t ret = 0;
	std::array<Position, 8> moves({ Position(2, 1), Position(2, -1), Position(-2, 1), Position(-2, -1), 
												Position(1, 2), Position(1, -2), Position(-1, 2), Position(-1, -2)});
	for (Position move : moves) {
		if (x + move.x > 7 || x + move.x < 0 || y + move.y > 7 || y + move.y < 0) 
			continue;
		if (!only_specific_piece || (board.getPiece(x+move.x, y+move.y).type == (uint8_t)to_check_for
				&& board.getPiece(x+move.x, y+move.y).is_white != is_white)) {
			int i = (x + move.x) * 8 + (y + move.y);
			ret |= (uint64_t)1 << i;
		}
	}
	return ret;
}
template uint64_t get_knight_attacked_tiles<false, Pieces::Empty>(Board&, int, int, bool);
template uint64_t get_knight_attacked_tiles<false, Pieces::Pawn>(Board&, int, int, bool);
template uint64_t get_knight_attacked_tiles<false, Pieces::Knight>(Board&, int, int, bool);
template uint64_t get_knight_attacked_tiles<false, Pieces::Bishop>(Board&, int, int, bool);
template uint64_t get_knight_attacked_tiles<false, Pieces::Rook>(Board&, int, int, bool);
template uint64_t get_knight_attacked_tiles<false, Pieces::Queen>(Board&, int, int, bool);
template uint64_t get_knight_attacked_tiles<false, Pieces::King>(Board&, int, int, bool);


uint64_t get_knight_attacked_tiles(Board& board, int x, int y) {
	return get_knight_attacked_tiles(board, x, y, board.getPiece(x, y).is_white);
}

void add_pawn_moves(Board& board, int x, int y, bool is_white, std::vector<Move>& res) {
	int mod = is_white ? -1 : 1;
	if ((mod == -1 && y <= 0) || (mod == 1 && y >= 7)) return;
	if (board.getPiece(x, y + mod).type == (int)Pieces::Empty) { // Push pawn one tile
		int is_promotion_white = (mod == -1) ? 1 : 0;
		if ((mod == -1 && y == 1) || (mod == 1 && y == 6)) { // pawn is white and on 7th rank
			res.push_back(Move(x, y, x, y + mod, Pieces::Knight, is_promotion_white));
			res.push_back(Move(x, y, x, y + mod, Pieces::Bishop, is_promotion_white));
			res.push_back(Move(x, y, x, y + mod, Pieces::Rook, is_promotion_white));
			res.push_back(Move(x, y, x, y + mod, Pieces::Queen, is_promotion_white));
		} else {
			res.push_back(Move(x, y, x, y + mod));
		}	

	    if ((mod == -1 && y == 6) || (mod == 1 && y == 1)) { // pawn is on start tile
			if (board.getPiece(x, y + 2 * mod).type == (int)Pieces::Empty) { // 2nd tile in front is free
				res.push_back(Move(x, y, x, y + 2 * mod));
			}
		}
	}
	std::optional<Position> enPassantPosRaw = board.get_en_passant_pos();
	Position enPassantPos = Position(-1, -1);
	if (enPassantPosRaw.has_value()) {
		enPassantPos = Position(enPassantPosRaw->x, enPassantPosRaw->y);
	}

	if (x <= 6) {
		if ((board.getPiece(x + 1, y + mod).type != (int)Pieces::Empty &&
			board.getPiece(x + 1, y + mod).is_white != board.getPiece(x, y).is_white)
			|| (enPassantPos.x == x + 1 && enPassantPos.y == y + mod))
		{
			int is_promotion_white = (mod == -1) ? 1 : 0;
			if ((mod == -1 && y == 1) || (mod == 1 && y == 6)) { // pawn is white and on 7th rank
				res.push_back(Move(x, y, x + 1, y + mod, Pieces::Knight, is_promotion_white));
				res.push_back(Move(x, y, x + 1, y + mod, Pieces::Bishop, is_promotion_white));
				res.push_back(Move(x, y, x + 1, y + mod, Pieces::Rook, is_promotion_white));
				res.push_back(Move(x, y, x + 1, y + mod, Pieces::Queen, is_promotion_white));
			} else {
				res.push_back(Move(x, y, x + 1, y + mod));
			}		
		}
	}
	if (x >= 1) {
		if ((board.getPiece(x - 1, y + mod).type != (int)Pieces::Empty &&
			board.getPiece(x - 1, y + mod).is_white != is_white)
			|| (enPassantPos.x == x - 1 && enPassantPos.y == y + mod))
		{
			int is_promotion_white = (mod == -1) ? 1 : 0;
			if ((mod == -1 && y == 1) || (mod == 1 && y == 6)) { // pawn is white and on 7th rank
				res.push_back(Move(x, y, x - 1, y + mod, Pieces::Knight, is_promotion_white));
				res.push_back(Move(x, y, x - 1, y + mod, Pieces::Bishop, is_promotion_white));
				res.push_back(Move(x, y, x - 1, y + mod, Pieces::Rook, is_promotion_white));
				res.push_back(Move(x, y, x - 1, y + mod, Pieces::Queen, is_promotion_white));
			} else {
				res.push_back(Move(x, y, x - 1, y + mod));
			}
		}
	}
}

void add_pawn_moves(Board& board, int x, int y,
		std::vector<Move>& res) {
	add_pawn_moves(board, x, y, board.getPiece(x, y).is_white, res);
}

uint64_t get_pawn_pushing_tiles(Board& board, int x, int y, bool is_white) {	
	uint64_t ret = 0;
	int mod = is_white ? -1 : 1;
	
	if (is_white && y == 0)
		return 0;
	else if (!is_white && y == 7)
		return 0;

	if ((is_white && y == 6) || (!is_white && y == 1)) {
		if (board.getPiece(x, y+2*mod).type == (uint8_t)Pieces::Empty) {
			int i = x * 8 + (y + 2*mod);
			ret |= (uint64_t)1 << i;
		}
	}

	if (board.getPiece(x, y+mod).type == (uint8_t)Pieces::Empty) {
		int i = x * 8 + (y + mod);
		ret |= (uint64_t)1 << i;
	}
	return ret;
}

template<bool only_specific_piece = false, Pieces to_check_for = Pieces::Empty>
uint64_t get_pawn_attacked_tiles(Board& board, int x, int y, bool is_white) {
	uint64_t ret = 0;
	int mod = is_white ? -1 : 1;
	
	if (is_white && y == 0)
		return 0;
	else if (!is_white && y == 7)
		return 0;

	if (x <= 6) {
		if (!only_specific_piece || (board.getPiece(x+1, y+mod).type == (uint8_t)to_check_for
				&& (bool)board.getPiece(x+1, y+mod).is_white != is_white)) {
			int i = (x + 1) * 8 + (y + mod);
			ret |= (uint64_t)1 << i;
		}
	}
	if (x >= 1) {
		if (!only_specific_piece || (board.getPiece(x-1, y+mod).type == (uint8_t)to_check_for
				&& (bool)board.getPiece(x-1, y+mod).is_white != is_white)) {
			int i = (x - 1) * 8 + (y + mod);
			ret |= (uint64_t)1 << i;
		}
	}
	return ret;
}
template uint64_t get_pawn_attacked_tiles<false, Pieces::Empty>(Board&, int, int, bool);
template uint64_t get_pawn_attacked_tiles<false, Pieces::Pawn>(Board&, int, int, bool);
template uint64_t get_pawn_attacked_tiles<false, Pieces::Knight>(Board&, int, int, bool);
template uint64_t get_pawn_attacked_tiles<false, Pieces::Bishop>(Board&, int, int, bool);
template uint64_t get_pawn_attacked_tiles<false, Pieces::Rook>(Board&, int, int, bool);
template uint64_t get_pawn_attacked_tiles<false, Pieces::Queen>(Board&, int, int, bool);
template uint64_t get_pawn_attacked_tiles<false, Pieces::King>(Board&, int, int, bool);

uint64_t get_pawn_attacked_tiles(Board& board, int x, int y) {
	return get_pawn_attacked_tiles(board, x, y, (bool)board.getPiece(x, y).is_white);
}

void add_king_moves(Board& board, int x, int y, bool is_white, std::vector<Move>& res) {
	std::vector<Position> mods = {
		Position(-1, -1), Position(-1, 0), Position(-1, 1),
		Position(0, -1),          Position{ 0, 1},
		Position(1, -1), Position(1, 0), Position(1, 1),
	};

	for (Position& mod : mods) {
		int dx = mod.x, dy = mod.y;
		int _x = x + dx, _y = y + dy;

		if (_x >= 0 && _x < 8 && _y >= 0 && _y < 8) {
			res.push_back(Move(x, y, _x, _y));
		}
	}
	
	if (is_white) {
		if (board.white_castle_long) {
			// three pieces to the left of the king are empty
			if (board.getPiece(1, 7).type + board.getPiece(2, 7).type + board.getPiece(3, 7).type == (int)Pieces::Empty) {
				if (!board.tile_is_attacked(4, 7, true) &&
					!board.tile_is_attacked(1, 7, true) && 
					!board.tile_is_attacked(2, 7, true) && 
					!board.tile_is_attacked(3, 7, true)) // tiles to castle over are not attacked
				{
					res.push_back(Move(x, y, 2, 7));
				}
			}
		}
		if (board.white_castle_short) {
			// two pieces to the right of the king are empty
			if (board.getPiece(5, 7).type + board.getPiece(6, 7).type == (int)Pieces::Empty) {
				if (!board.tile_is_attacked(4, 7, true) &&
					!board.tile_is_attacked(5, 7, true) &&
					!board.tile_is_attacked(6, 7, true)) // tiles to castle over are not attacked
				{
					res.push_back(Move(x, y, 6, 7));
				}
			}
		}
	} else {
		if (board.black_castle_long) {
			// three pieces to the left of the king are empty
			if (board.getPiece(1, 0).type + board.getPiece(2, 0).type + board.getPiece(3, 0).type == (int)Pieces::Empty) {
				if (!board.tile_is_attacked(4, 0, true) &&
					!board.tile_is_attacked(1, 0, true) && 
					!board.tile_is_attacked(2, 0, true) && 
					!board.tile_is_attacked(3, 0, true)) // tiles to castle over are not attacked
				{
					res.push_back(Move(x, y, 2, 0));
				}
			}
		}
		if (board.black_castle_short) {
			// two pieces to the right of the king are empty
			if (board.getPiece(5, 0).type + board.getPiece(6, 0).type == (int)Pieces::Empty) {
				if (!board.tile_is_attacked(4, 0, true) &&
					!board.tile_is_attacked(5, 0, true) && 
					!board.tile_is_attacked(6, 0, true)) // tiles to castle over are not attacked
				{
					res.push_back(Move(x, y, 6, 0));
				}
			}
		}
	}
}

void add_king_moves(Board& board, int x, int y, std::vector<Move>& res) {
	add_king_moves(board, x, y, board.getPiece(x, y).is_white, res);
}

template<bool only_specific_piece = false, Pieces to_check_for = Pieces::Empty>
uint64_t get_king_attacked_tiles(Board& board, int x, int y, bool is_white) {
	uint64_t ret = 0;
	std::vector<Position> mods = {
		Position(-1, -1), Position(-1, 0), Position(-1, 1),
		Position(0, -1),          Position{ 0, 1},
		Position(1, -1), Position(1, 0), Position(1, 1),
	};

	for (Position& mod : mods) {
		int dx = mod.x, dy = mod.y;
		int _x = x + dx, _y = y + dy;

		if (_x >= 0 && _x < 8 && _y >= 0 && _y < 8) {
			if (!only_specific_piece || (board.getPiece(_x, _y).type == (uint8_t)to_check_for
					&& board.getPiece(_x, _y).is_white != is_white)) {
				int i = _x * 8 + _y;
				ret |= (uint64_t)1 << i;
			}
		}
	}
	return ret;
}
template uint64_t get_king_attacked_tiles<false, Pieces::Empty>(Board&, int, int, bool);
template uint64_t get_king_attacked_tiles<false, Pieces::Pawn>(Board&, int, int, bool);
template uint64_t get_king_attacked_tiles<false, Pieces::Knight>(Board&, int, int, bool);
template uint64_t get_king_attacked_tiles<false, Pieces::Bishop>(Board&, int, int, bool);
template uint64_t get_king_attacked_tiles<false, Pieces::Rook>(Board&, int, int, bool);
template uint64_t get_king_attacked_tiles<false, Pieces::Queen>(Board&, int, int, bool);
template uint64_t get_king_attacked_tiles<false, Pieces::King>(Board&, int, int, bool);


uint64_t get_king_attacked_tiles(Board& board, int x, int y) {
	return get_king_attacked_tiles(board, x, y, board.getPiece(x, y).is_white);
}

uint64_t Piece::get_moves_raw_bit(Board& board, int x, int y) {
	uint64_t ret = 0;
	switch ((Pieces)type) {
	case Pieces::Pawn:
		ret = get_pawn_pushing_tiles(board, x, y, board.white_to_move);
		break;

	case Pieces::Knight:
		ret = get_knight_attacked_tiles(board, x, y, board.white_to_move);
		break;

	case Pieces::Bishop:
		ret = get_bishop_attacked_tiles(board, x, y, board.white_to_move);
		break;

	case Pieces::Rook:
		ret = get_rook_attacked_tiles(board, x, y, board.white_to_move);
		break;

	case Pieces::Queen:
		ret = get_rook_attacked_tiles(board, x, y, board.white_to_move);
		ret |= get_bishop_attacked_tiles(board, x, y, board.white_to_move);
		break;

	case Pieces::King: 
		ret = get_king_attacked_tiles(board, x, y, board.white_to_move);
		break;

	default:
		return 0;
	}
	return ret;
}

void Piece::get_moves_raw(Board& board, int x, int y, std::vector<Move>& res) {
	res.clear();
	switch ((Pieces)type) {
	case Pieces::Pawn:
		add_pawn_moves(board, x, y, res);
		break;

	case Pieces::Knight:
		add_knight_moves(x, y, res);
		break;

	case Pieces::Bishop:
		add_bishop_moves(board, x, y, res);
		break;

	case Pieces::Rook:
		add_rook_moves(board, x, y, res);
		break;

	case Pieces::Queen:
		add_rook_moves(board, x, y, res);
		add_bishop_moves(board, x, y, res);
		break;

	case Pieces::King: 
		add_king_moves(board, x, y, res);
		break;

	default:
		break;
	}
}

uint64_t Piece::get_attacked_tiles(Board& board, int x, int y) {
	return get_attacked_tiles(board, x, y, false);
}
uint64_t Piece::get_attacked_tiles(Board& board, int x, int y, int ignore_king) { // include kings: -1, ignore white king: 0, ignore black king: 1
	uint64_t ret = 0;
	switch ((Pieces)type) {
	case Pieces::Pawn:
		ret |= get_pawn_attacked_tiles(board, x, y);
		break;

	case Pieces::Knight:
		ret |= get_knight_attacked_tiles(board, x, y);
		break;

	case Pieces::Bishop:
		ret |= get_bishop_attacked_tiles(board, x, y, ignore_king);
		break;

	case Pieces::Rook:
		ret |= get_rook_attacked_tiles(board, x, y, ignore_king);
		break;

	case Pieces::Queen:
		ret |= get_rook_attacked_tiles(board, x, y, ignore_king);
		ret |= get_bishop_attacked_tiles(board, x, y, ignore_king);
		break;

	case Pieces::King: 
		ret |= get_king_attacked_tiles(board, x, y);
		break;

	default:
		break;
	}
	return ret;
}

uint64_t Piece::get_king_attackers(Board& board, bool is_white) {
	Position king_pos = is_white ? board.white_king : board.black_king;
	uint64_t ret = 0;
	
	ret |= get_pawn_attacked_tiles<true, Pieces::Pawn>(board, king_pos.x, king_pos.y, is_white);
	ret |= get_knight_attacked_tiles<true, Pieces::Knight>(board, king_pos.x, king_pos.y, is_white);
	ret |= get_bishop_attacked_tiles<true, Pieces::Bishop>(board, king_pos.x, king_pos.y, is_white, -1);
	ret |= get_rook_attacked_tiles<true, Pieces::Rook>(board, king_pos.x, king_pos.y, is_white, -1);
	ret |= get_rook_attacked_tiles<true, Pieces::Queen>(board, king_pos.x, king_pos.y, is_white, -1); //check bishop and rook moves again but with queen in template
	ret |= get_bishop_attacked_tiles<true, Pieces::Queen>(board, king_pos.x, king_pos.y, is_white, -1);
	ret |= get_king_attacked_tiles<true, Pieces::King>(board, king_pos.x, king_pos.y, is_white);

	return ret;
}

bool Piece::type_is_slider(uint8_t t) {
	return t != (int)Pieces::Pawn && t != (int)Pieces::Knight;
}


uint64_t Piece::get_sliding_ray(Position from, Position to) {
	uint64_t ret = 0;
	if (from.x == to.x) { // on same file
		if (from.y > to.y) {
			for (int i = to.y + 1; i < from.y; i++) {
				ret |= (uint64_t)1 << (from.x * 8 + i);
			}
		} else {
			for (int i = from.y + 1; i < to.y; i++) {
				ret |= (uint64_t)1 << (from.x * 8 + i);
			}
		}
	} else if (from.y == to.y) { // on same rank
		if (from.x > to.x) {
			for (int i = to.x + 1; i < from.x; i++) {
				ret |= (uint64_t)1 << (i * 8 + from.y);
			}
		} else {
			for (int i = from.x + 1; i < to.x; i++) {
				ret |= (uint64_t)1 << (i * 8 + from.y);
			}
		}
	} else if (std::abs(from.x - to.x) == std::abs(from.y - to.y)) { // on same diagonal
		if (from.x > to.x) { // diagonal to the left
			if (from.y > to.y) { // diagonal to the top left
				for (int i = 1; i < std::abs(from.x - to.x); i++) {
					ret |= (uint64_t)1 << ((from.x - i) * 8 + (from.y - i));
				}
			} else { // diagonal to the bottom left
				for (int i = 1; i < std::abs(from.x - to.x); i++) {
					ret |= (uint64_t)1 << ((from.x - i) * 8 + (from.y + i));
				}
			}
		} else { // diagonal to the right
			if (from.y > to.y) { // to the top right
				for (int i = 1; i < std::abs(from.x - to.x); i++) {
					ret |= (uint64_t)1 << ((from.x + i) * 8 + (from.y - i));
				}
			} else { // to the bottom right
				for (int i = 1; i < std::abs(from.x - to.x); i++) {
					ret |= (uint64_t)1 << ((from.x + i) * 8 + (from.y + i));
				}
			}
		}
	}
	return ret;
}
