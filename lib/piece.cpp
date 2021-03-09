#include "piece.hpp"

// _is_white has to be 0 or 1
Piece::Piece(uint8_t _type, uint8_t _is_white) {
	type = _type;
	is_white = _is_white;
}
