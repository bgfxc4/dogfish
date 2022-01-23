#pragma once

#include <bits/stdint-uintn.h>
#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <vector>
#include <cstring>

#include "piece.hpp"
#include "constants.hpp"

class BoardContent {
public:
	BoardContent() {
		memset(bits, 0, sizeof(bits));
	}
	
	Piece get(unsigned x, unsigned y) {
		return get(y * 8 + x);
	}

	uint8_t* get_all_raw() { return bits;}

	void set(int x, int y, Piece p) {
		set(y * 8 + x, p.type | (p.is_white << 3));
	}

	void clear_tile(int x, int y) {
		set(y * 8 + x, 0 | (1 << 3));
	}

private:
	uint8_t bits[32];
	Piece get(unsigned n) {
		uint8_t byte = bits[n >> 1];
		byte >>= (n & 1) << 2;
		byte &= 0x0F;
		return { (uint8_t)(byte & 0x07), (uint8_t)(byte >> 3) };
	}

	void set(uint8_t n, uint8_t p) {
		if (n & 1) {
			bits[n >> 1] &= 0x0F;
			bits[n >> 1] |= p << 4;
		}
		else {
			bits[n >> 1] &= 0xF0;
			bits[n >> 1] |= p;
		}
	}
};

class Position {
	public:
	int x;
	int y;

	Position(int _x, int _y) : x(_x), y(_y) {}
	bool operator== (Position second);
	bool operator!= (Position second);
};

class Move {
	public:
	int8_t from_x, from_y;
	int8_t to_x, to_y;
	Pieces promote_to = Pieces::Empty; // Pieces::Empty if no promotion
	int promotion_is_white = 0;

	Move(int _from_x, int _from_y, int _to_x, int _to_y)
		: from_x(_from_x), from_y(_from_y), to_x(_to_x), to_y(_to_y) {}

	Move(int _from_x, int _from_y, int _to_x, int _to_y,
			Pieces _promote_to, int _promotion_is_white)
		: from_x(_from_x), from_y(_from_y), to_x(_to_x), to_y(_to_y),
		promote_to(_promote_to), promotion_is_white(_promotion_is_white) {}

	bool operator== (Move& second);
};

enum GameState {
	playing,
	draw,
	white_checkmate,
	black_checkmate,
};

class BoardLite {
	public:
	BoardLite(Board* board);
	uint8_t* get_all_raw();
	
	BoardContent bc;
	uint8_t white_to_move : 1;
	uint8_t white_castle_short : 1;
	uint8_t white_castle_long : 1;
	uint8_t black_castle_short : 1;
	uint8_t black_castle_long : 1;
	uint8_t _en_passant_pos : 7;
};

class Board {
public:
	GameState gameState = GameState::playing;
	
	Position white_king = Position(-1, -1);
	Position black_king = Position(-1, -1);

	uint8_t white_to_move : 1;
		
	uint8_t white_castle_short : 1;
	uint8_t white_castle_long : 1;
	uint8_t black_castle_short : 1;
	uint8_t black_castle_long : 1;

	// order of these fields is important for alignment. total size should be 36 bytes
	BoardContent bc;
	uint8_t _en_passant_pos : 7;
	
	uint64_t attacked_tiles = 0;
	uint64_t pinned_pieces = 0;
	uint64_t attacked_tiles_ign_king = 0;
	uint64_t king_attackers = 0;
	std::vector<Move> all_possible_moves;
	std::vector<BoardLite> whole_game;

	Board(const std::string& fenString);
	Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {}
	std::optional<Position> get_en_passant_pos();
	void set_en_passant_pos(Position pos);
	void clear_en_passant_pos();
	Piece getPiece(int x, int y);

	std::vector<Move> get_moves(int x, int y);
	bool tile_is_attacked(int x, int y);
	bool tile_is_attacked(int x, int y, bool ignoreKings);
	bool is_check();
	bool is_check_slow();
	bool is_insufficient_material();
	uint8_t* get_all_raw();
	void add_position_to_whole_game();
	void find_kings();

	uint64_t calculate_pinned_pieces();
	uint64_t get_king_danger_tiles(bool is_white);
	void calculate_all_possible_moves();
	
	bool is_same_position(BoardLite& board);

	void move_raw(int from_x, int from_y, int to_x, int to_y); // semi-private; don't use
	void move(Move move);

	void print();
	
	private:
	uint8_t num_half_moves;
	uint16_t num_moves : 12;

	int parseFenString(const std::string& fenString);
	void get_moves_raw(int x, int y, std::vector<Move>& res);
	uint64_t get_moves_raw_bit(int x, int y);
};
