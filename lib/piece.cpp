#include <iostream>

#include "piece.hpp"
#include "board.hpp"
#include "constants.hpp"

// _is_white has to be 0 or 1
Piece::Piece(uint8_t _type, uint8_t _is_white) {
	type = _type;
	is_white = _is_white;
}

static void add_rook_moves(Board& board, int x, int y,
		std::vector<Move>& res)
{
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

static void add_rook_attacked_tiles(Board& board, int x, int y,
		std::vector<Position>& res)
{
	for (int _x = x - 1; _x >= 0; _x--) {
		res.push_back(Position(_x, y));
		if (board.getPiece(_x, y).type != (uint8_t)Pieces::Empty) break;
	}
	for (int _x = x + 1; _x < 8; _x++) {
		res.push_back(Position(_x, y));
		if (board.getPiece(_x, y).type != (uint8_t)Pieces::Empty) break;
	}
	for (int _y = y - 1; _y >= 0; _y--) {
		res.push_back(Position(x, _y));
		if (board.getPiece(x, _y).type != (uint8_t)Pieces::Empty) break;
	}
	for (int _y = y + 1; _y < 8; _y++) {
		res.push_back(Position(x, _y));
		if (board.getPiece(x, _y).type != (uint8_t)Pieces::Empty) break;
	}
}

static void add_bishop_moves(Board& board, int x, int y,
		std::vector<Move>& res)
{
	std::vector<Position> mods = { Position(-1, -1), Position(-1, 1), Position(1, -1), Position(1, 1)};

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

static void add_bishop_attacked_tiles(Board& board, int x, int y,
		std::vector<Position>& res)
{
	std::vector<Position> mods = { Position(-1, -1), Position(-1, 1), Position(1, -1), Position(1, 1)};

	for (Position mod : mods) {
		int dx = mod.x, dy = mod.y;
		for (int _y = y + dy, _x = x + dx;
				_y >= 0 && _y < 8 && _x >= 0 && _x < 8;
				_y += dy, _x += dx)
		{
			res.push_back(Position(_x, _y));
			if (board.getPiece(_x, _y).type != (uint8_t)Pieces::Empty) break;
		}
	}
}

static void add_knight_moves(int x, int y, std::vector<Move>& res)
{
	std::vector<Position> moves = { Position(2, 1), Position(2, -1), Position(-2, 1), Position(-2, -1), 
												Position(1, 2), Position(1, -2), Position(-1, 2), Position(-1, -2)};
	for (Position move : moves) {
		if (x + move.x > 7 || x + move.x < 0 || y + move.y > 7 || y + move.y < 0) continue;
		res.push_back(Move(x, y, x + move.x, y + move.y));
	}
}

static void add_knight_attacked_tiles(int x, int y, std::vector<Position>& res)
{
	std::vector<Position> moves = { Position(2, 1), Position(2, -1), Position(-2, 1), Position(-2, -1), 
												Position(1, 2), Position(1, -2), Position(-1, 2), Position(-1, -2)};
	for (Position move : moves) {
		if (x + move.x > 7 || x + move.x < 0 || y + move.y > 7 || y + move.y < 0) continue;
		res.push_back(Position(x + move.x, y + move.y));
	}
}

static void add_pawn_moves(Board& board, int x, int y, std::vector<Move>& res) {
	int mod = (board.getPiece(x, y).is_white) ? -1 : 1;
	if ((mod == -1 && y <= 0) || (mod == 1 && y >= 7)) return;
	if (board.getPiece(x, y + mod).type == (int)Pieces::Empty) { // Push pawn one tile
		int is_promotion_white = (mod == -1) ? 1 : 0;
		if ((mod == -1 && y == 1) || (mod == 1 && y == 6)) { // pawn is white and on 7th rank
			res.push_back(Move(x, y, x, y + mod, (int)Pieces::Knight, is_promotion_white));
			res.push_back(Move(x, y, x, y + mod, (int)Pieces::Bishop, is_promotion_white));
			res.push_back(Move(x, y, x, y + mod, (int)Pieces::Rook, is_promotion_white));
			res.push_back(Move(x, y, x, y + mod, (int)Pieces::Queen, is_promotion_white));
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
				res.push_back(Move(x, y, x + 1, y + mod, (int)Pieces::Knight, is_promotion_white));
				res.push_back(Move(x, y, x + 1, y + mod, (int)Pieces::Bishop, is_promotion_white));
				res.push_back(Move(x, y, x + 1, y + mod, (int)Pieces::Rook, is_promotion_white));
				res.push_back(Move(x, y, x + 1, y + mod, (int)Pieces::Queen, is_promotion_white));
			} else {
				res.push_back(Move(x, y, x + 1, y + mod));
			}		
		}
	}
	if (x >= 1) {
		if ((board.getPiece(x - 1, y + mod).type != (int)Pieces::Empty &&
			board.getPiece(x - 1, y + mod).is_white != board.getPiece(x, y).is_white)
			|| (enPassantPos.x == x - 1 && enPassantPos.y == y + mod))
		{
			int is_promotion_white = (mod == -1) ? 1 : 0;
			if ((mod == -1 && y == 1) || (mod == 1 && y == 6)) { // pawn is white and on 7th rank
				res.push_back(Move(x, y, x - 1, y + mod, (int)Pieces::Knight, is_promotion_white));
				res.push_back(Move(x, y, x - 1, y + mod, (int)Pieces::Bishop, is_promotion_white));
				res.push_back(Move(x, y, x - 1, y + mod, (int)Pieces::Rook, is_promotion_white));
				res.push_back(Move(x, y, x - 1, y + mod, (int)Pieces::Queen, is_promotion_white));
			} else {
				res.push_back(Move(x, y, x - 1, y + mod));
			}
		}
	}
}

static void add_pawn_attacked_tiles(Board& board, int x, int y, std::vector<Position>& res) {
	Piece p = board.getPiece(x, y);
	int mod = (p.is_white) ? -1 : 1;

	if (x <= 6) {
		if (board.getPiece(x + 1, y + mod).is_white != p.is_white || board.getPiece(x + 1, y + mod).type == (int)Pieces::Empty) {
			res.push_back(Position(x + 1, y + mod));
		}
	}
	if (x >= 1) {
		if (board.getPiece(x - 1, y + mod).is_white != p.is_white || board.getPiece(x - 1, y + mod).type == (int)Pieces::Empty) {
			res.push_back(Position(x - 1, y + mod));
		}
	}
}

static void add_king_moves(Board& board, int x, int y, std::vector<Move>& res) {
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
	
	Piece p = board.getPiece(x, y);
	if (p.is_white) {
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

static void add_king_attacked_tiles(int x, int y, std::vector<Position>& res) {
	std::vector<Position> mods = {
		Position(-1, -1), Position(-1, 0), Position(-1, 1),
		Position(0, -1),          Position{ 0, 1},
		Position(1, -1), Position(1, 0), Position(1, 1),
	};

	for (Position& mod : mods) {
		int dx = mod.x, dy = mod.y;
		int _x = x + dx, _y = y + dy;

		if (_x >= 0 && _x < 8 && _y >= 0 && _y < 8) {
			res.push_back(Position(_x, _y));
		}
	}
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

void Piece::get_attacked_tiles(Board& board, int x, int y, std::vector<Position>& res) {
	res.clear();
	switch ((Pieces)type) {
	case Pieces::Pawn:
		add_pawn_attacked_tiles(board, x, y, res);
		break;

	case Pieces::Knight:
		add_knight_attacked_tiles(x, y, res);
		break;

	case Pieces::Bishop:
		add_bishop_attacked_tiles(board, x, y, res);
		break;

	case Pieces::Rook:
		add_rook_attacked_tiles(board, x, y, res);
		break;

	case Pieces::Queen:
		add_rook_attacked_tiles(board, x, y, res);
		add_bishop_attacked_tiles(board, x, y, res);
		break;

	case Pieces::King: 
		add_king_attacked_tiles(x, y, res);
		break;

	default:
		break;
	}
}
