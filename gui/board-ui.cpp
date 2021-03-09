#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <vector>
#include <iostream>

#include "piece.hpp"
#include "board.hpp"
#include "board-ui.hpp"
#include "graphics.hpp"

int* tileNameToPosition(const std::string& tileName) {
	static int ret[2] = { -1, -1 };

	if (tileName.size() != 2) {
		std::cout << "[ERROR] tileName must be 2 characters long!" << std::endl;
		return ret;
	}
	ret[0] = tileName[0] - 'a';
	ret[1] = '8' - tileName[1];

	return ret;
}

BoardUI::BoardUI() {
	selectedTileWhiteTexture.loadFromFile("images/selectedTileWhite.png");
	selectedTileBlackTexture.loadFromFile("images/selectedTileBlack.png");
	possibleMoveTexture.loadFromFile("images/possibleMove.png");
	possibleTakeTexture.loadFromFile("images/possibleTake.png");
	selectedTileWhiteSprite = sf::Sprite(selectedTileWhiteTexture);
	selectedTileBlackSprite = sf::Sprite(selectedTileBlackTexture);
	possibleMoveSprite = sf::Sprite(possibleMoveTexture);
	possibleTakeSprite = sf::Sprite(possibleTakeTexture);
}
BoardUI::~BoardUI() {
}

void BoardUI::renderBoard(sf::RenderWindow& window, Board& boardToRender) {
	renderSelectedTiles(window);
	renderPieces(window, boardToRender);
	if (selectedTile[0] != -1 && selectedTile[1] != -1)
		renderPossibleMoves(window);
}

void BoardUI::renderSelectedTiles(sf::RenderWindow& window) {
	if (selectedTile[0] == -1 && selectedTile[1] == -1)
		return;
	if ((selectedTile[0] + selectedTile[1]) % 2 == 0) {
		selectedTileWhiteSprite.setPosition(selectedTile[0] * 75, selectedTile[1] * 75);
		window.draw(selectedTileWhiteSprite);
	} else {
		selectedTileBlackSprite.setPosition(selectedTile[0] * 75, selectedTile[1] * 75);
		window.draw(selectedTileBlackSprite);
	}
}

void BoardUI::renderPossibleMoves(sf::RenderWindow& window) {
	/*std::vector<sf::Vector2i> possibleMoves = tiles[selectedTile[0]][selectedTile[1]]->calculateAllMoves();
	for (sf::Vector2i move : possibleMoves) {
		if (tiles[move.x][move.y]->figure == nullptr && !tiles[move.x][move.y]->enPassantPossible) {
			possibleMoveSprite.setPosition(move.x * 75, move.y * 75);
			window.draw(possibleMoveSprite);
		} else {
			possibleTakeSprite.setPosition(move.x * 75, move.y * 75);
			window.draw(possibleTakeSprite);
		}
	}*/
}

void BoardUI::renderPieces(sf::RenderWindow& window, Board& boardToRender) {
	int draggedSpriteIndex = -1;
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece toRender = boardToRender.getPiece(x, y);
			if (toRender.type == 0) continue;
			int offset = toRender.is_white ? 0 : 6;
			if (dragStartPos.x == x && dragStartPos.y == y) {
				draggedSpriteIndex = offset + (toRender.type - 1);
			} else {
				figuresSprites[offset + (toRender.type - 1)].setPosition(x * 75, y * 75);
				window.draw(figuresSprites[offset + (toRender.type - 1)]);

			}
		}
	}
	if (draggedSpriteIndex != -1) {
		sf::Vector2i mousepPos = sf::Mouse::getPosition(window);
		figuresSprites[draggedSpriteIndex].setPosition(mousepPos.x - (75/2), mousepPos.y - (75/2));
		window.draw(figuresSprites[draggedSpriteIndex]);
	}
}

void BoardUI::startMouseClick(sf::Vector2i mousePos, Board& board) {
	dragStartPos = { -1, -1 };
	if ((mousePos.x / 75) >= 8 || (mousePos.y / 75) >= 8)
		return;
	if (board.getPiece(mousePos.x / 75, mousePos.y / 75).type == 0)
		return;
	dragStartPos.x = mousePos.x / 75;
	dragStartPos.y = mousePos.y / 75;	
}

void BoardUI::endMouseClick(sf::Vector2i mousePos, Board& board) {
	dragStartPos = { -1, -1 };

	if ((mousePos.x / 75) >= 8 || (mousePos.y / 75) >= 8)
		return;
	//move(dragStartPos.x / 75, dragStartPos.y / 75, mousePos.x / 75, mousePos.y / 75);
}
