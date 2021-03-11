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
		std::vector<std::pair<int, int>>& res)
{
	for (int _x = x - 1; _x >= 0; _x--) {
		res.push_back({_x, y});
		if (board.getPiece(_x, y).type != (uint8_t)Pieces::Empty) break;
	}
	for (int _x = x + 1; _x < 8; _x++) {
		res.push_back({_x, y});
		if (board.getPiece(_x, y).type != (uint8_t)Pieces::Empty) break;
	}
	for (int _y = y - 1; _y >= 0; _y--) {
		res.push_back({x, _y});
		if (board.getPiece(x, _y).type != (uint8_t)Pieces::Empty) break;
	}
	for (int _y = y + 1; _y < 8; _y++) {
		res.push_back({x, _y});
		if (board.getPiece(x, _y).type != (uint8_t)Pieces::Empty) break;
	}
}

static void add_bishop_moves(Board& board, int x, int y,
		std::vector<std::pair<int, int>>& res)
{
	std::vector<std::pair<int, int>> mods = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

	for (std::pair<int, int> mod : mods) {
		int dx = std::get<0>(mod), dy = std::get<1>(mod);
		for (int _y = y + dy, _x = x + dx;
				_y >= 0 && _y < 8 && _x >= 0 && _x < 8;
				_y += dy, _x += dx)
		{
			res.push_back({_x, _y});
			if (board.getPiece(_x, _y).type != (uint8_t)Pieces::Empty) break;
		}
	}
}

static void add_knight_moves(Board& board, int x, int y,
		std::vector<std::pair<int, int>>& res)
{
	std::vector<std::pair<int, int>> moves = { {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, 
												{1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
	for (std::pair<int,int> move : moves) {
		if (x + move.first > 7 || x + move.first < 0 || y + move.second > 7 || y + move.second < 0) continue;
		res.push_back({x + move.first, y + move.second});
	}
}

static void add_pawn_moves(Board& board, int x, int y, std::vector<std::pair<int, int>>& res) {
	int mod = (board.getPiece(x, y).is_white) ? -1 : 1;
	if ((mod == -1 && y <= 0) || (mod == 1 && y >= 7)) return;
	if (board.getPiece(x, y + mod).type == (int)Pieces::Empty) { // Push pawn one tile
		res.push_back({x, y + mod});
	    if ((mod == -1 && y == 6) || (mod == 1 && y == 1)) { // pawn is on start tile
			if (board.getPiece(x, y + 2 * mod).type == (int)Pieces::Empty) { // 2nd tile in front is free
			res.push_back({x, y + 2 * mod});
			}
		}
	}
	if (x <= 6) {
		if (board.getPiece(x + 1, y + mod).type != (int)Pieces::Empty &&
			board.getPiece(x + 1, y + mod).is_white != board.getPiece(x, y).is_white)
		{
			res.push_back({x + 1, y + mod});
		}
	}
	if (x >= 1) {
		if (board.getPiece(x - 1, y + mod).type != (int)Pieces::Empty &&
			board.getPiece(x - 1, y + mod).is_white != board.getPiece(x, y).is_white)
		{
			res.push_back({x - 1, y + mod});
		}
	}
}

static void add_king_moves(Board& board, int x, int y, std::vector<std::pair<int, int>>& res) {
	std::vector<std::pair<int, int>> mods = {
		{-1, -1}, {-1, 0}, {-1, 1},
		{ 0, -1},          { 0, 1},
		{ 1, -1}, { 1, 0}, { 1, 1},
	};

	for (std::pair<int, int>& mod : mods) {
		int dx = std::get<0>(mod), dy = std::get<1>(mod);
		int _x = x + dx, _y = y + dy;

		if (_x >= 0 && _x < 8 && _y >= 0 && _y < 8) {
			res.push_back({_x, _y});
		}
	}
	
	Piece p = board.getPiece(x, y);
	if (p.is_white) {
		if (board.white_castle_long) {
			// three pieces to the left of the king are empty
			if (board.getPiece(1, 7).type + board.getPiece(2, 7).type + board.getPiece(3, 7).type == (int)Pieces::Empty) {
				if (!board.tile_is_attacked(!p.is_white, 4, 7, true) &&
					!board.tile_is_attacked(!p.is_white, 1, 7, true) && 
					!board.tile_is_attacked(!p.is_white, 2, 7, true) && 
					!board.tile_is_attacked(!p.is_white, 3, 7, true)) // tiles to castle over are not attacked
				{
					res.push_back({2, 7});
				}
			}
		}
		if (board.white_castle_short) {
			// two pieces to the right of the king are empty
			if (board.getPiece(5, 7).type + board.getPiece(6, 7).type == (int)Pieces::Empty) {
				if (!board.tile_is_attacked(!p.is_white, 4, 7, true) &&
					!board.tile_is_attacked(!p.is_white, 5, 7, true) &&
					!board.tile_is_attacked(!p.is_white, 6, 7, true)) // tiles to castle over are not attacked
				{
					res.push_back({6, 7});
				}
			}
		}
	} else {
		if (board.black_castle_long) {
			// three pieces to the left of the king are empty
			if (board.getPiece(1, 0).type + board.getPiece(2, 0).type + board.getPiece(3, 0).type == (int)Pieces::Empty) {
				if (!board.tile_is_attacked(!p.is_white, 4, 0, true) &&
					!board.tile_is_attacked(!p.is_white, 1, 0, true) && 
					!board.tile_is_attacked(!p.is_white, 2, 0, true) && 
					!board.tile_is_attacked(!p.is_white, 3, 0, true)) // tiles to castle over are not attacked
				{
					res.push_back({2, 0});
				}
			}
		}
		if (board.black_castle_short) {
			// two pieces to the right of the king are empty
			if (board.getPiece(5, 0).type + board.getPiece(6, 0).type == (int)Pieces::Empty) {
				if (!board.tile_is_attacked(!p.is_white, 4, 0, true) &&
					!board.tile_is_attacked(!p.is_white, 5, 0, true) && 
					!board.tile_is_attacked(!p.is_white, 6, 0, true)) // tiles to castle over are not attacked
				{
					res.push_back({6, 0});
				}
			}
		}
	}
}

std::vector<std::pair<int, int>> Piece::get_moves_raw(Board& board, int x, int y) {
	std::vector<std::pair<int, int>> res;
	switch ((Pieces)type) {
	case Pieces::Pawn:
		add_pawn_moves(board, x, y, res);
		break;

	case Pieces::Knight:
		add_knight_moves(board, x, y, res);
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

	return res;
}
