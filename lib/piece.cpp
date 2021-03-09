#include "piece.hpp"

#include "board.hpp"
#include "constants.hpp"

// _is_white has to be 0 or 1
Piece::Piece(uint8_t _type, uint8_t _is_white) {
	type = _type;
	is_white = _is_white;
}

std::vector<std::pair<int, int>> Piece::get_moves_raw(Board& board, int x, int y) {
	std::vector<std::pair<int, int>> res;
	switch ((Pieces)type) {
	case Pieces::Rook:
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
		break;

	case Pieces::Bishop: {
		std::vector<std::pair<int, int>> mods = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

		for (std::pair<int, int> mod : mods) {
			int mx = std::get<0>(mod), my = std::get<1>(mod);
			for (int _y = y + my, _x = x + mx;
					_y >= 0 && _y < 8 && _x >= 0 && _x < 8;
					_y += my, _x += mx)
			{
				res.push_back({_x, _y});
				if (board.getPiece(_x, _y).type != (uint8_t)Pieces::Empty) break;
			}
		}
	}
		break;


	default:
		break;
	}

	return res;
}
