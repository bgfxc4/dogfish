#pragma once

#include <string>
#include <cstdint>
#include <SFML/Graphics.hpp>

#include "tile.hpp"
#include "piece.hpp"
#include "enums.hpp"

class Board {
	public:
	std::array<std::array<Tile*, 8>, 8> tiles;
	std::array<int8_t, 2> selectedTile = { -1, -1 };

	sf::Texture selectedTileWhiteTexture = sf::Texture();
	sf::Texture selectedTileBlackTexture = sf::Texture();
	sf::Texture possibleMoveTexture = sf::Texture();
	sf::Texture possibleTakeTexture = sf::Texture();
	sf::Sprite selectedTileWhiteSprite;
	sf::Sprite selectedTileBlackSprite;
	sf::Sprite possibleMoveSprite;
	sf::Sprite possibleTakeSprite;

	Color toMove = Color::White;
	int halfMoves = 0; //moves in a row, where no pawn was moved or no pice was taken
	int moveCount = 1; //starts at 1, +1 after black moved
	bool whiteCanCastleLong : 1 = false;
	bool whiteCanCastleShot : 1 = false;
	bool blackCanCastleLong : 1 = false;
	bool blackCanCastleShot : 1 = false;

	Figure* draggedFigure = nullptr;
	sf::Vector2i dragStartPos;

	Board();
	~Board();

	void renderBoard(sf::RenderWindow& window);
	void renderSelectedTiles(sf::RenderWindow& window);
	void renderPossibleMoves(sf::RenderWindow& window);
	void renderFigures(sf::RenderWindow& window);

	void move(int startX, int startY, int endX, int endY);

	void startMouseClick(sf::Vector2i mousePos);
	void endMouseClick(sf::Vector2i mousePos);

	int parseFEN(const std::string& fenString);
	std::string getFEN();
};
