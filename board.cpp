#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <iostream>

#include "tile.hpp"
#include "figure.hpp"
#include "board.hpp"

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

Board::Board() {
	selectedTileWhiteTexture.loadFromFile("images/selectedTileWhite.png");
	selectedTileBlackTexture.loadFromFile("images/selectedTileBlack.png");
	possibleMoveTexture.loadFromFile("images/possibleMove.png");
	possibleTakeTexture.loadFromFile("images/possibleTake.png");
	selectedTileWhiteSprite = sf::Sprite(selectedTileWhiteTexture);
	selectedTileBlackSprite = sf::Sprite(selectedTileBlackTexture);
	possibleMoveSprite = sf::Sprite(possibleMoveTexture);
	possibleTakeSprite = sf::Sprite(possibleTakeTexture);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			tiles[i][j] = new Tile(i, j, &tiles);
		}
	}
	parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq h6 0 1");
}
Board::~Board() {
	for (std::array<Tile*, 8>& vec : tiles) {
		for (Tile* tile : vec) {
			delete tile;
		}
	}
}

void Board::renderBoard(sf::RenderWindow& window) {
	renderSelectedTiles(window);
	renderFigures(window);
	if (selectedTile[0] != -1 && selectedTile[1] != -1)
		renderPossibleMoves(window);
}

void Board::renderSelectedTiles(sf::RenderWindow& window) {
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

void Board::renderPossibleMoves(sf::RenderWindow& window) {
	std::vector<sf::Vector2i> possibleMoves = tiles[selectedTile[0]][selectedTile[1]]->calculateAllMoves();
	for (sf::Vector2i move : possibleMoves) {
		if (tiles[move.x][move.y]->figure == nullptr && !tiles[move.x][move.y]->enPassantPossible) {
			possibleMoveSprite.setPosition(move.x * 75, move.y * 75);
			window.draw(possibleMoveSprite);
		} else {
			possibleTakeSprite.setPosition(move.x * 75, move.y * 75);
			window.draw(possibleTakeSprite);
		}
	}
}

void Board::renderFigures(sf::RenderWindow& window) {
	for (int x = 0; x < (int)tiles.size(); x++) {
		for (int y = 0; y < (int)tiles[x].size(); y++) {
			if (tiles[x][y]->figure == nullptr) continue;
			tiles[x][y]->renderFigure(window);
		}
	}

	if (draggedFigure == nullptr)
		return;
	window.draw(*draggedFigure->sprite);
}

void Board::move(int startX, int startY, int endX, int endY) {
	if (startX == endX && startY == endY)
		return;

	Tile* startTile = tiles[startX][startY];
	Tile* endTile = tiles[endX][endY];

	if (endTile->enPassantPossible) { //en Passant happened
		if (startTile->figure->type == Pieces::WhitePawn) {
			delete tiles[endX][endY + 1]->figure;
			tiles[endX][endY + 1]->figure = nullptr;
		} else if (startTile->figure->type == Pieces::BlackPawn) {
			delete tiles[endX][endY - 1]->figure;
			tiles[endX][endY - 1]->figure = nullptr;
		}

	}

	for (std::array<Tile*, 8>& vec : tiles) {
		for (Tile* tile : vec) {
			tile->move(startTile, endTile);
		}
	}

	delete endTile->figure;
	endTile->setFigure(startTile->figure);
	startTile->figure = nullptr;
	selectedTile = { -1, -1 };
}

void Board::startMouseClick(sf::Vector2i mousePos) {
	selectedTile = { -1, -1 };
	if ((mousePos.x / 75) >= (int)tiles.size() || (mousePos.y / 75) >= (int)tiles[0].size())
		return;
	if (tiles[mousePos.x / 75][mousePos.y / 75]->figure == nullptr)
		return;
	selectedTile[0] = mousePos.x / 75;
	selectedTile[1] = mousePos.y / 75;
	draggedFigure = tiles[mousePos.x / 75][mousePos.y / 75]->figure;
	dragStartPos = mousePos;
	draggedFigure->isDragged = true;
}

void Board::endMouseClick(sf::Vector2i mousePos) {
	if (draggedFigure == nullptr)
		return;

	draggedFigure->isDragged = false;
	draggedFigure = nullptr;

	if ((mousePos.x / 75) >= (int)tiles.size() || (mousePos.y / 75) >= (int)tiles[0].size())
		return;
	move(dragStartPos.x / 75, dragStartPos.y / 75, mousePos.x / 75, mousePos.y / 75);
}

int Board::parseFEN(const std::string& fenString) {
	char temp_char = 0; // declaration can't be after first goto

	const char* c = fenString.c_str() - 1;

	// positions
	int row = 0, col = 0;
	while (1) {
		if (!*++c)
			goto err;
		Pieces type = Pieces::Empty;
		if (*c == '/') {
			row++;
			col = 0;
		} else if (std::isdigit(*c))
			col += *c - '0';
		else if (*c == 'R' || *c == 'r')
			type = (*c == 'R') ? Pieces::WhiteRook : Pieces::BlackRook;
		else if (*c == 'N' || *c == 'n')
			type = (*c == 'N') ? Pieces::WhiteKnight : Pieces::BlackKnight;
		else if (*c == 'B' || *c == 'b')
			type = (*c == 'B') ? Pieces::WhiteBishop : Pieces::BlackBishop;
		else if (*c == 'Q' || *c == 'q')
			type = (*c == 'Q') ? Pieces::WhiteQueen :Pieces::BlackQueen;
		else if (*c == 'K' || *c == 'k')
			type = (*c == 'K') ? Pieces::WhiteKing : Pieces::BlackKing;
		else if (*c == 'P' || *c == 'p')
			type = (*c == 'P') ? Pieces::WhitePawn : Pieces::BlackPawn;
		else if (*c == ' ')
			break;
		else {
			std::cout << "[ERROR] invalid fen string at 1st group c: " << c << std::endl;
			return -1;
		}

		if (type != Pieces::Empty) {
			Figure* figure = new Figure(type);
			tiles[col][row]->setFigure(figure);
			col++;
		}
	}

	// who moves next
	while (1) {
		if (!*++c)
			goto err;
		if (*c == 'w')
			toMove = Color::White;
		else if (*c == 'b')
			toMove = Color::Black;
		else if (*c == ' ')
			break;
		else {
			std::cout << "[ERROR] invalid fen string at 2nd group" << std::endl;
			return -1;
		}
	}

	// who can castle
	while (1) {
		if (!*++c)
			goto err;
		if (*c == 'Q')
			whiteCanCastleLong = true;
		else if (*c == 'K')
			whiteCanCastleShot = true;
		else if (*c == 'q')
			blackCanCastleLong = true;
		else if (*c == 'k')
			blackCanCastleShot = true;
		else if (*c == ' ')
			break;
		else {
			std::cout << "[ERROR] invalid fen string at 3rd group" << std::endl;
			return -1;
		}
	}

	// possible en passant positions
	while (1) {
		if (!*++c)
			goto err;
		if (*c == '-')
			continue;
		else if (*c == ' ')
			break;
		else if (!std::isdigit(*c))
			temp_char = *c;
		else if (std::isdigit(*c)) {
			if (temp_char == 0) {
				std::cout << "[ERROR] invalid fen string at 4th group(1)" << std::endl;
				return -1;
			} else {
				std::string tileName = std::string(1, temp_char) + *c;
				int* enPassantTile = tileNameToPosition(tileName);
				tiles[enPassantTile[0]][enPassantTile[1]]->enPassantPossible = true;
				temp_char = 0;
			}
		} else {
			std::cout << "[ERROR] invalid fen string at 4th group(2)" << std::endl;
			return -1;
		}
	}

	// half-move counter
	halfMoves = 0;
	while (1) {
		if (!*++c)
			goto err;

		if (std::isdigit(*c))
			halfMoves = halfMoves * 10 + (*c - '0');
		else if (*c == ' ')
			break;
		else {
			std::cout << "[ERROR] invalid fen string at 5th group" << std::endl;
			return -1;
		}
	}

	// move counter
	moveCount = 0;
	while (1) {
		if (!*++c || !(*c-' ')) // mega kek
			break;
		else if (std::isdigit(*c))
			moveCount = moveCount * 10 + (*c - '0');
		else {
			std::cout << "[ERROR] invalid fen string at 6th group" << std::endl;
			return -1;
		}
	}

	return 0;

err:
	std::cout << "[ERROR] fen string too short" << std::endl;
	return -1;
}

std::string Board::getFEN() {
	//TODO
	return "";
}
