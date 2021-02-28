#pragma once

#include <string>
#include <SFML/Graphics.hpp>

#include "tile.hpp"
#include "figure.hpp"
#include "enums.hpp"

class Board {
	public:
	std::array<std::array<Tile*, 8>, 8> tiles;
	std::array<int, 2> selectedTile = { -1, -1 };
	Color toMove = Color::White;
	int halfMoves = 0; //moves in a row, where no pawn was moved or no pice was taken
	int moveCount = 1; //starts at 1, +1 after black moved
	bool whiteCanCastleLong = false;
	bool whiteCanCastleShot = false;
	bool blackCanCastleLong = false;
	bool blackCanCastleShot = false;

	sf::Texture selectedTileWhiteTexture = sf::Texture();
	sf::Texture selectedTileBlackTexture = sf::Texture();
	sf::Texture possibleMoveTexture = sf::Texture();
	sf::Texture possibleTakeTexture = sf::Texture();
	sf::Sprite selectedTileWhiteSprite;
	sf::Sprite selectedTileBlackSprite;
	sf::Sprite possibleMoveSprite;
	sf::Sprite possibleTakeSprite;

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

	int parseFEN(std::string fenString);
	std::string getFEN();
};
