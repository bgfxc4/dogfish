#include <SFML/Graphics.hpp>
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
	renderFigures(window, boardToRender);
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

void BoardUI::renderFigures(sf::RenderWindow& window, Board& boardToRender) {
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece toRender = boardToRender.getPiece(x, y);
			if (toRender.type == 0) continue;
			int offset = toRender.is_white ? 0 : 6;
			figuresSprites[offset + (toRender.type - 1)].setPosition(x * 75, y * 75);
			window.draw(figuresSprites[offset + (toRender.type - 1)]);
		}
	}
/*	for (int x = 0; x < (int)tiles.size(); x++) {
		for (int y = 0; y < (int)tiles[x].size(); y++) {
			if (tiles[x][y]->figure == nullptr) continue;
			tiles[x][y]->renderFigure(window);
		}
	}

	if (draggedFigure == nullptr)
		return;
	window.draw(*draggedFigure->sprite);*/
}

void BoardUI::startMouseClick(sf::Vector2i mousePos) {
	/*selectedTile = { -1, -1 };
	if ((mousePos.x / 75) >= (int)tiles.size() || (mousePos.y / 75) >= (int)tiles[0].size())
		return;
	if (tiles[mousePos.x / 75][mousePos.y / 75]->figure == nullptr)
		return;
	selectedTile[0] = mousePos.x / 75;
	selectedTile[1] = mousePos.y / 75;
	draggedFigure = tiles[mousePos.x / 75][mousePos.y / 75]->figure;
	dragStartPos = mousePos;
	draggedFigure->isDragged = true;*/
}

void BoardUI::endMouseClick(sf::Vector2i mousePos) {
	/*if (draggedFigure == nullptr)
		return;

	draggedFigure->isDragged = false;
	draggedFigure = nullptr;

	if ((mousePos.x / 75) >= (int)tiles.size() || (mousePos.y / 75) >= (int)tiles[0].size())
		return;
	move(dragStartPos.x / 75, dragStartPos.y / 75, mousePos.x / 75, mousePos.y / 75);*/
}
