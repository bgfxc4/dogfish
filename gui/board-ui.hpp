#pragma once

#include <string>
#include <SFML/Graphics.hpp>

#include "piece.hpp"
#include "constants.hpp"
#include "board.hpp"

class BoardUI {
	public:
	std::array<int8_t, 2> selectedTile = { -1, -1 };
	bool isCheck = false;

	sf::Texture selectedTileWhiteTexture = sf::Texture();
	sf::Texture selectedTileBlackTexture = sf::Texture();
	sf::Texture possibleMoveTexture = sf::Texture();
	sf::Texture possibleTakeTexture = sf::Texture();
	sf::Texture isCheckTileTexture = sf::Texture();
	sf::Sprite selectedTileWhiteSprite;
	sf::Sprite selectedTileBlackSprite;
	sf::Sprite possibleMoveSprite;
	sf::Sprite possibleTakeSprite;
	sf::Sprite isCheckTileSprite;

	sf::Vector2i dragStartPos = {-1, -1};
	sf::Sprite* draggedSprite;

	BoardUI();
	~BoardUI();

	void renderBoard(sf::RenderWindow& window, Board& boardToRender);
	void renderSelectedTiles(sf::RenderWindow& window);
	void renderPossibleMoves(sf::RenderWindow& window, Board& board);
	void renderPieces(sf::RenderWindow& window, Board& boardToRender);

	void startMouseClick(sf::Vector2i mousePos, Board& board);
	void endMouseClick(sf::Vector2i mousePos, Board& board);

	void tryMove(Board& board, int fromX, int fromY, int toX, int toY);
};
