#include <SFML/System/Vector2.hpp>
#include <array>
#include <vector>
#include <SFML/Graphics.hpp>

#include "enums.hpp"
#include "figure.hpp"
#include "board.hpp"
#include "graphics.hpp"
#include "tile.hpp"

Tile::Tile(int x, int y, std::array<std::array<Tile*, 8>, 8>* tiles) {
	position.x = x;
	position.y = y;
	this->tiles = tiles;
	this->figure = nullptr;
}

Tile::~Tile() {
	if (figure != nullptr)
		delete figure;
}

void Tile::setFigure(Figure* toSet) {
	figure = toSet;
}

void Tile::move(Tile* startTile, Tile* endTile) {
	if (startTile->figure->type == Pieces::WhitePawn && startTile->position.y - endTile->position.y == 2) { //white moved a pawn 2 tiles forward, en passant is possible
		tiles->at(endTile->position.x).at(endTile->position.y + 1)->enPassantPossible = true;
		if (position.y == endTile->position.y + 1 && position.x == endTile->position.x) {
			enPassantPossible = true; //set enPassantPossible true on the tile between start and end
		} else {
			enPassantPossible = false; //on all others set enPassantPossible false
		}
	} else if (startTile->figure->type == Pieces::BlackPawn && endTile->position.y - startTile->position.y == 2) { //black moved a pawn 2 tiles forward, en passant is possible
		tiles->at(endTile->position.x).at(endTile->position.y - 1)->enPassantPossible = true;
		if (position.y == endTile->position.y - 1 && position.x == endTile->position.x) {
			enPassantPossible = true; //set enPassantPossible true on the tile between start and end
		} else {
			enPassantPossible = false; //on all others set enPassantPossible false
		}
	} else {
		enPassantPossible = false; //set enPassantPossible on all tiles false if no pawn was moved 2 tiles forward
	}
}

std::vector<sf::Vector2i> calculateAllPawnMoves(Figure* figure, std::array<std::array<Tile*, 8>, 8>* tiles, sf::Vector2i& position) {
	std::vector<sf::Vector2i> possibleMoves;
	if (figure->type == Pieces::WhitePawn) { //calculate all moves for a white pawn
		if (position.y != 0) {
			if (tiles->at(position.x).at(position.y - 1)->figure == nullptr) {
				possibleMoves.push_back(sf::Vector2i(position.x, position.y - 1));
				if (position.y == 6 && tiles->at(position.x).at(4)->figure == nullptr) { //has not moved and next two squares are free
					possibleMoves.push_back((sf::Vector2i(position.x, position.y - 2)));
				}
			}
			if (position.x != 0) { //not completly left
				if (tiles->at(position.x - 1).at(position.y - 1)->figure != nullptr) {
					if (tiles->at(position.x - 1).at(position.y - 1)->figure->getColor() == Color::Black) {
						possibleMoves.push_back(sf::Vector2i(position.x - 1, position.y - 1));
					}
				} else if (tiles->at(position.x - 1).at(position.y - 1)->enPassantPossible) {
					if (tiles->at(position.x - 1).at(position.y)->figure != nullptr) {
						if (tiles->at(position.x - 1).at(position.y)->figure->getColor() == Color::Black) possibleMoves.push_back(sf::Vector2i(position.x - 1, position.y - 1));
					}
				}
			}
			if (position.x != 7) { //not completly right
				if (tiles->at(position.x + 1).at(position.y - 1)->figure != nullptr) {
					if (tiles->at(position.x + 1).at(position.y - 1)->figure->getColor() == Color::Black) {
						possibleMoves.push_back(sf::Vector2i(position.x + 1, position.y - 1));
					}
				} else if (tiles->at(position.x + 1).at(position.y - 1)->enPassantPossible) {
					if (tiles->at(position.x + 1).at(position.y)->figure != nullptr) {
						if (tiles->at(position.x + 1).at(position.y)->figure->getColor() == Color::Black) possibleMoves.push_back(sf::Vector2i(position.x + 1, position.y - 1));
					}
				}
			}
		}
	} else if (figure->type == Pieces::BlackPawn) { //calculate all moves for a black pawn
		if (position.y != 7) {
			if (tiles->at(position.x).at(position.y + 1)->figure == nullptr) {
				possibleMoves.push_back(sf::Vector2i(position.x, position.y + 1));
				if (position.y == 1 && tiles->at(position.x).at(3)->figure == nullptr) { //has not moved and next two squares are free
					possibleMoves.push_back((sf::Vector2i(position.x, position.y + 2)));
				}
			}
			if (position.x != 0) { //not completly left
				if (tiles->at(position.x - 1).at(position.y + 1)->figure != nullptr) {
					if (tiles->at(position.x - 1).at(position.y + 1)->figure->getColor() == Color::White) {
						possibleMoves.push_back(sf::Vector2i(position.x - 1, position.y + 1));
					}
				} else if (tiles->at(position.x - 1).at(position.y + 1)->enPassantPossible) {
					if (tiles->at(position.x + 1).at(position.y)->figure != nullptr) {
						if (tiles->at(position.x + 1).at(position.y)->figure->getColor() == Color::White) possibleMoves.push_back(sf::Vector2i(position.x - 1, position.y + 1));
					}
				}
			}
			if (position.x != 7) { //not completly right
				if (tiles->at(position.x + 1).at(position.y + 1)->figure != nullptr) {
					if (tiles->at(position.x + 1).at(position.y + 1)->figure->getColor() == Color::White) {
						possibleMoves.push_back(sf::Vector2i(position.x + 1, position.y + 1));
					}
				} else if (tiles->at(position.x + 1).at(position.y + 1)->enPassantPossible) {
					if (tiles->at(position.x + 1).at(position.y)->figure != nullptr) {
						if (tiles->at(position.x + 1).at(position.y)->figure->getColor() == Color::White) possibleMoves.push_back(sf::Vector2i(position.x + 1, position.y + 1));
					}
				}
			}
		}
	}
	return possibleMoves;
}

std::vector<sf::Vector2i> calculateAllKnightMoves(std::array<std::array<Tile*, 8>, 8>* tiles, sf::Vector2i& position) {
	std::vector<sf::Vector2i> possibleMoves;
	if (position.x >= 1) {
		if (position.y <= 5) {
			possibleMoves.push_back(sf::Vector2i(position.x - 1, position.y + 2));
		}
		if (position.y >= 2) {
			possibleMoves.push_back(sf::Vector2i(position.x - 1, position.y - 2));
		}
	}
	if (position.x >= 2) {
		if (position.y <= 6) {
			possibleMoves.push_back(sf::Vector2i(position.x - 2, position.y + 1));
		}
		if (position.y >= 1) {
			possibleMoves.push_back(sf::Vector2i(position.x - 2, position.y - 1));
		}
	}
	if (position.x <= 6) {
		if (position.y <= 5) {
			possibleMoves.push_back(sf::Vector2i(position.x + 1, position.y + 2));
		}
		if (position.y >= 2) {
			possibleMoves.push_back(sf::Vector2i(position.x + 1, position.y - 2));
		}
	}
	if (position.x <= 5) {
		if (position.y <= 6) {
			possibleMoves.push_back(sf::Vector2i(position.x + 2, position.y + 1));
		}
		if (position.y >= 1) {
			possibleMoves.push_back(sf::Vector2i(position.x + 2, position.y - 1));
		}
	}
	return possibleMoves;
}

std::vector<sf::Vector2i> Tile::calculateAllMoves() {
	std::vector<sf::Vector2i> possibleMoves;
	if (figure->type == Pieces::WhitePawn || figure->type == Pieces::BlackPawn) {
		possibleMoves = calculateAllPawnMoves(figure, tiles, position);	
	} else if (figure->type == Pieces::WhiteKnight || figure->type == Pieces::BlackKnight) {
		possibleMoves = calculateAllKnightMoves(tiles, position);
	}
	return possibleMoves;
}

void Tile::renderFigure(sf::RenderWindow& window) {
	if (!figure->isDragged)
		figure->sprite->setPosition(position.x * 75, position.y * 75);
	else {
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		figure->sprite->setPosition(mousePos.x - 37.5f, mousePos.y - 37.5f);
	}
	window.draw(*(figure->sprite));
}
