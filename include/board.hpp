#pragma once

#include <bits/stdint-uintn.h>
#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "piece.hpp"

class BoardContent {
public:
	BoardContent();
	Piece get(int x, int y);
	void set(int x, int y, Piece p);
	void clear_tile(int x, int y);

private:
	uint8_t bits[32];
	Piece get(uint8_t n);
	void set(uint8_t n, uint8_t p);
};

class Position {
	public:
	int x;
	int y;

	Position(int x, int y);
	bool operator== (Position second);
};

class Move {
	public:
	int to_x;
	int to_y;
	int is_promotion = 0; // if the move isn*t a promotion, this is 0, if it is, the int has the value of a piece from the Pieces enum
	int promotion_is_white = 0;
	Move(int to_x, int to_y, int is_promotion, int promotion_is_white);
	Move(int to_x, int to_y);
	bool operator== (Move& second);
};

class Board {
public:
	uint8_t white_to_move : 1;
	
	uint8_t white_castle_short : 1;
	uint8_t white_castle_long : 1;
	uint8_t black_castle_short : 1;
	uint8_t black_castle_long : 1;
	
	Board(const std::string& fenString);
	Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {}
	std::optional<Position> get_en_passant_pos();
	void set_en_passant_pos(Position pos);
	void clear_en_passant_pos();
	Piece getPiece(int x, int y);

	std::vector<Move> get_moves(int x, int y);
	bool tile_is_attacked(uint8_t color /* white: 0, black: 1 */, int x, int y);
	bool tile_is_attacked(uint8_t color /* white: 0, black: 1 */, int x, int y, bool ignoreKings);
	bool is_check();
	bool tile_is_attacked_straight_diagonal(uint8_t color /* white: 0, black: 1 */, int x, int y);

	void move_raw(int from_x, int from_y, int to_x, int to_y); // semi-private; don't use
	void move(int from_x, int from_y, Move move);

private:
	// order of these fields is important for alignment. total size should be 36 bytes
	BoardContent bc;
	uint8_t _en_passant_pos : 7;

	uint8_t num_half_moves;
	uint16_t num_moves : 12;

	int parseFenString(const std::string& fenString);
	std::vector<Move> get_moves_raw(int x, int y);
};
