#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <sys/types.h>
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
	isCheckTileTexture.loadFromFile("images/isCheckTile.png");
	selectedTileWhiteSprite = sf::Sprite(selectedTileWhiteTexture);
	selectedTileBlackSprite = sf::Sprite(selectedTileBlackTexture);
	possibleMoveSprite = sf::Sprite(possibleMoveTexture);
	possibleTakeSprite = sf::Sprite(possibleTakeTexture);
	isCheckTileSprite = sf::Sprite(isCheckTileTexture);
}
BoardUI::~BoardUI() {
}

void BoardUI::renderBoard(sf::RenderWindow& window, Board& boardToRender) {
	renderSelectedTiles(window);
	renderPossibleMoves(window, boardToRender);
	renderPieces(window, boardToRender);
}

void BoardUI::renderSelectedTiles(sf::RenderWindow& window) {
	if (selectedTile[0] < 0 || selectedTile[1] < 0)
		return;
	if ((selectedTile[0] + selectedTile[1]) % 2 == 0) {
		selectedTileWhiteSprite.setPosition(selectedTile[0] * 75, selectedTile[1] * 75);
		window.draw(selectedTileWhiteSprite);
	} else {
		selectedTileBlackSprite.setPosition(selectedTile[0] * 75, selectedTile[1] * 75);
		window.draw(selectedTileBlackSprite);
	}
}

void BoardUI::renderPossibleMoves(sf::RenderWindow& window, Board& board) {
	std::optional<Position> enPassantPosRaw = board.get_en_passant_pos();
	Position enPassantPos = Position(-1, -1 );
	if (enPassantPosRaw.has_value()) {
		enPassantPos = { enPassantPosRaw->x, enPassantPosRaw->y };
	}

	if (selectedTile[0] < 0 || selectedTile[1] < 0) return;
	std::vector<Move> possibleMoves = board.get_moves(selectedTile[0], selectedTile[1]);
	for (Move move : possibleMoves) {
		if (board.getPiece(move.to_x, move.to_y).type == (int)Pieces::Empty && 
			(move.to_x != enPassantPos.x || move.to_y != enPassantPos.y)) 
		{
			possibleMoveSprite.setPosition(move.to_x * 75, move.to_y * 75);
			window.draw(possibleMoveSprite);
		} else {
			possibleTakeSprite.setPosition(move.to_x * 75, move.to_y * 75);
			window.draw(possibleTakeSprite);
		}
	}
}

void BoardUI::renderPieces(sf::RenderWindow& window, Board& board) {
	int draggedSpriteIndex = -1;
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece toRender = board.getPiece(x, y);
			if (toRender.type == (int)Pieces::Empty) continue;
			if (toRender.type == (u_int8_t)Pieces::King && toRender.is_white == board.white_to_move && isCheck) {
				isCheckTileSprite.setPosition(x * 75, y * 75);
				window.draw(isCheckTileSprite);
			}

			int offset = toRender.is_white ? 0 : 6;
			if (dragStartPos.x == x && dragStartPos.y == y) {
				draggedSpriteIndex = offset + (toRender.type);
			} else {
				figuresSprites[offset + (toRender.type)].setPosition(x * 75, y * 75);
				window.draw(figuresSprites[offset + (toRender.type)]);
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
	selectedTile = { -1, -1 };
	if ((mousePos.x / 75) >= 8 || (mousePos.y / 75) >= 8)
		return;

	if (board.getPiece(mousePos.x / 75, mousePos.y / 75).type != (int)Pieces::Empty) {
		selectedTile[0] = mousePos.x / 75;
		selectedTile[1] = mousePos.y / 75;
	}

	if (board.getPiece(mousePos.x / 75, mousePos.y / 75).type == 0) {
		return;
	}
	dragStartPos.x = mousePos.x / 75;
	dragStartPos.y = mousePos.y / 75;
}

void BoardUI::endMouseClick(sf::Vector2i mousePos, Board& board) {
	if ((mousePos.x / 75) >= 8 || (mousePos.y / 75) >= 8) {
		dragStartPos = { -1, -1 };
		return;
	}
	if (dragStartPos.x < 0 || dragStartPos.y < 0) {
		dragStartPos = { -1, -1 };
		return;
	}
	if (dragStartPos.x == mousePos.x / 75 && dragStartPos.y == mousePos.y / 75) {
		dragStartPos = { -1, -1 };
		return;
	}
	tryMove(board, dragStartPos.x, dragStartPos.y, mousePos.x / 75, mousePos.y / 75);
	dragStartPos = { -1, -1 };
}

void BoardUI::tryMove(Board& board, int fromX, int fromY, int toX, int toY) {

	std::vector<Move> possibleMoves = board.get_moves(fromX, fromY);
	Move move(-1, -1);

	for (Move possibleMove : possibleMoves) {
		if (possibleMove.to_x == toX && possibleMove.to_y == toY) {
			move = possibleMove;
		}
	}
	
	if (move.to_x == -1 || move.to_y == -1) return;

	board.move(fromX, fromY, move);
	isCheck = board.is_check();
}
