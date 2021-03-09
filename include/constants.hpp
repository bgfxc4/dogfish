#pragma once

enum class Pieces {
	Empty,
	Pawn,
	Bishop,
	Knight,
	Rook,
	Queen,
	King,

	// has to be <= 8. otherwise, the binary format of BoardContent.bits would no
	// longer be correct
	NumPieces
};

enum class Color {
	White,
	Black
};
