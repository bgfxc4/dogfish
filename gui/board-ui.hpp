#pragma once

#include <string>
#include <SFML/Graphics.hpp>

#include "piece.hpp"
#include "constants.hpp"
#include "board.hpp"

class BoardUI {
	public:
	std::array<int8_t, 2> selectedTile = { -1, -1 };

	sf::Texture selectedTileWhiteTexture = sf::Texture();
	sf::Texture selectedTileBlackTexture = sf::Texture();
	sf::Texture possibleMoveTexture = sf::Texture();
	sf::Texture possibleTakeTexture = sf::Texture();
	sf::Sprite selectedTileWhiteSprite;
	sf::Sprite selectedTileBlackSprite;
	sf::Sprite possibleMoveSprite;
	sf::Sprite possibleTakeSprite;

	sf::Vector2i dragStartPos;

	BoardUI();
	~BoardUI();

	void renderBoard(sf::RenderWindow& window, Board& boardToRender);
	void renderSelectedTiles(sf::RenderWindow& window);
	void renderPossibleMoves(sf::RenderWindow& window);
	void renderFigures(sf::RenderWindow& window, Board& boardToRender);

	void startMouseClick(sf::Vector2i mousePos);
	void endMouseClick(sf::Vector2i mousePos);
};
