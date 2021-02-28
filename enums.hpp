#pragma once

//this order is important (see Figure::getColor)
enum class Pieces {
	Empty,
	WhitePawn,
	WhiteBishop,
	WhiteKnight,
	WhiteRook,
	WhiteKing,
	WhiteQueen,
	BlackPawn,
	BlackBishop,
	BlackKnight,
	BlackRook,
	BlackKing,
	BlackQueen
};
enum class Color {
	White,
	Black,
	NoColor
};
