#pragma once

#include <string>
#include <SFML/Graphics.hpp>

#include "piece.hpp"
#include "constants.hpp"
#include "board.hpp"

enum UI_state {
	in_game,
	white_choosing_promotion,
	black_choosing_promotion,
};

class BoardUI {
	public:

	UI_state ui_state = UI_state::in_game;
	Position promotingPosition = Position(-1, -1);
	Position promotingPositionFrom = Position(-1, -1);
	Pieces promotingTo = Pieces::Empty;

	std::array<int8_t, 2> selectedTile = { -1, -1 };
	bool isCheck = false;

	sf::Texture selectedTileWhiteTexture = sf::Texture();
	sf::Texture selectedTileBlackTexture = sf::Texture();
	sf::Texture possibleMoveTexture = sf::Texture();
	sf::Texture possibleTakeTexture = sf::Texture();
	sf::Texture isCheckTileTexture = sf::Texture();
	sf::Texture promotionChooseWhiteTexture = sf::Texture();
	sf::Texture promotionChooseBlackTexture = sf::Texture();
	sf::Sprite selectedTileWhiteSprite;
	sf::Sprite selectedTileBlackSprite;
	sf::Sprite possibleMoveSprite;
	sf::Sprite possibleTakeSprite;
	sf::Sprite isCheckTileSprite;
	sf::Sprite promotionChooseWhiteSprite;
	sf::Sprite promotionChooseBlackSprite;

	sf::Vector2i dragStartPos = {-1, -1};
	sf::Sprite* draggedSprite;

	BoardUI();
	~BoardUI();

	void renderBoard(sf::RenderWindow& window, Board& boardToRender);
	void renderSelectedTiles(sf::RenderWindow& window);
	void renderPossibleMoves(sf::RenderWindow& window, Board& board);
	void renderPieces(sf::RenderWindow& window, Board& boardToRender);
	void renderPromotionChoose(sf::RenderWindow& window);

	void startMouseClick(sf::Vector2i mousePos, Board& board);
	void startMouseClickNormalState(sf::Vector2i mousePos, Board& board);
	void startMouseClickPromoteState(sf::Vector2i mousePos);

	void endMouseClick(sf::Vector2i mousePos, Board& board);
	void endMouseClickNormalState(sf::Vector2i mousePos, Board& board);
	void endMouseClickPromoteState(sf::Vector2i mousePos, Board& board);

	void tryMove(Board& board, int fromX, int fromY, int toX, int toY);
};
