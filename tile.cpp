#include <SFML/System/Vector2.hpp>
#include <array>
#include <vector>
#include <SFML/Graphics.hpp>

#include "enums.hpp"
#include "piece.hpp"
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
	int mults[] = { 1, -1 };
	int mult = mults[startTile->figure->type == Pieces::BlackPawn];

	if (startTile->position.y - endTile->position.y == 2 * mult) {
		(*tiles)[endTile->position.x][endTile->position.y + 1 * mult]->enPassantPossible = true;

		if (position.y == endTile->position.y + 1 * mult && position.x == endTile->position.x) {
			// set enPassantPossible true on the tile between start and end
			enPassantPossible = true;
		} else {
			// on all others set enPassantPossible false
			enPassantPossible = false;
		}
	} else {
		// set enPassantPossible on all tiles false if no pawn was moved 2 tiles forward
		enPassantPossible = false;
	}
}

std::vector<sf::Vector2i> calculateAllWhitePawnMoves(std::array<std::array<Tile*, 8>, 8>* tiles, const sf::Vector2i& position) {
	std::vector<sf::Vector2i> possibleMoves;
	if (position.y == 0) // pawn is at the top of the board; no further moves
		return {};

	if ((*tiles)[position.x][position.y - 1]->figure == nullptr) {
		possibleMoves.emplace_back(position.x, position.y - 1);

		if (position.y == 6 && (*tiles)[position.x][4]->figure == nullptr) { // first move; can move 2 squares
			possibleMoves.emplace_back(position.x, position.y - 2);
		}
	}
	if (position.x != 0) { //not completely left
		if ((*tiles)[position.x - 1][position.y - 1]->figure != nullptr) {
			if ((*tiles)[position.x - 1][position.y - 1]->figure->getColor() == Color::Black) {
				possibleMoves.emplace_back(position.x - 1, position.y - 1);
			}
		} else if ((*tiles)[position.x - 1][position.y - 1]->enPassantPossible) {
			if ((*tiles)[position.x - 1][position.y]->figure != nullptr) {
				if ((*tiles)[position.x - 1][position.y]->figure->getColor() == Color::Black) {
					possibleMoves.emplace_back(position.x - 1, position.y - 1);
				}
			}
		}
	}
	if (position.x != 7) { //not completely right
		if ((*tiles)[position.x + 1][position.y - 1]->figure != nullptr) {
			if ((*tiles)[position.x + 1][position.y - 1]->figure->getColor() == Color::Black) {
				possibleMoves.emplace_back(position.x + 1, position.y - 1);
			}
		} else if ((*tiles)[position.x + 1][position.y - 1]->enPassantPossible) {
			if ((*tiles)[position.x + 1][position.y]->figure != nullptr) {
				if ((*tiles)[position.x + 1][position.y]->figure->getColor() == Color::Black) {
					possibleMoves.emplace_back(position.x + 1, position.y - 1);
				}
			}
		}
	}
	return possibleMoves;
}

std::vector<sf::Vector2i> calculateAllBlackPawnMoves(std::array<std::array<Tile*, 8>, 8>* tiles, sf::Vector2i& position) {
	std::vector<sf::Vector2i> possibleMoves;
	if (position.y != 7) {
		if ((*tiles)[position.x][position.y + 1]->figure == nullptr) {
			possibleMoves.push_back(sf::Vector2i(position.x, position.y + 1));
			if (position.y == 1 && (*tiles)[position.x][3]->figure == nullptr) { //has not moved and next two squares are free
				possibleMoves.push_back((sf::Vector2i(position.x, position.y + 2)));
			}
		}
		if (position.x != 0) { //not completely left
			if ((*tiles)[position.x - 1][position.y + 1]->figure != nullptr) {
				if ((*tiles)[position.x - 1][position.y + 1]->figure->getColor() == Color::White) {
					possibleMoves.push_back(sf::Vector2i(position.x - 1, position.y + 1));
				}
			} else if ((*tiles)[position.x - 1][position.y + 1]->enPassantPossible) {
				if ((*tiles)[position.x + 1][position.y]->figure != nullptr) {
					if ((*tiles)[position.x + 1][position.y]->figure->getColor() == Color::White) possibleMoves.push_back(sf::Vector2i(position.x - 1, position.y + 1));
				}
			}
		}
		if (position.x != 7) { //not completely right
			if ((*tiles)[position.x + 1][position.y + 1]->figure != nullptr) {
				if ((*tiles)[position.x + 1][position.y + 1]->figure->getColor() == Color::White) {
					possibleMoves.push_back(sf::Vector2i(position.x + 1, position.y + 1));
				}
			} else if ((*tiles)[position.x + 1][position.y + 1]->enPassantPossible) {
				if ((*tiles)[position.x + 1][position.y]->figure != nullptr) {
					if ((*tiles)[position.x + 1][position.y]->figure->getColor() == Color::White) possibleMoves.push_back(sf::Vector2i(position.x + 1, position.y + 1));
				}
			}
		}
	}
	return possibleMoves;
}

std::vector<sf::Vector2i> calculateAllKnightMoves(std::array<std::array<Tile*, 8>, 8>* tiles, sf::Vector2i& position) {
	std::vector<sf::Vector2i> tmp;
	tmp.push_back(sf::Vector2i(position.x - 1, position.y + 2));
	tmp.push_back(sf::Vector2i(position.x - 1, position.y - 2));
	tmp.push_back(sf::Vector2i(position.x - 2, position.y + 1));
	tmp.push_back(sf::Vector2i(position.x - 2, position.y - 1));
	tmp.push_back(sf::Vector2i(position.x + 1, position.y + 2));
	tmp.push_back(sf::Vector2i(position.x + 1, position.y - 2));
	tmp.push_back(sf::Vector2i(position.x + 2, position.y + 1));
	tmp.push_back(sf::Vector2i(position.x + 2, position.y - 1));

	std::vector<sf::Vector2i> possibleMoves;
	for (sf::Vector2i& m : tmp) {
		if (m.x >= 0 && m.x < 8 && m.y >= 0 && m.y < 8) {
			if ((*tiles)[m.x][m.y]->figure != nullptr) {
				if ((*tiles)[m.x][m.y]->figure->getColor() == (*tiles)[position.x][position.y]->figure->getColor()) {
					continue;
				}
			}
			possibleMoves.push_back(std::move(m));
		}
	}

	return possibleMoves;
}

std::vector<sf::Vector2i> calculateAllBishopMoves(std::array<std::array<Tile*, 8>, 8>* tiles, sf::Vector2i& position) {

}

std::vector<sf::Vector2i> Tile::calculateAllMoves() {
	std::vector<sf::Vector2i> possibleMoves;
	if (figure->type == Pieces::WhitePawn) {
		possibleMoves = calculateAllWhitePawnMoves(tiles, position);	
	} else if (figure->type == Pieces::BlackPawn) {
		possibleMoves = calculateAllBlackPawnMoves(tiles, position);
	}else if (figure->type == Pieces::WhiteKnight || figure->type == Pieces::BlackKnight) {
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
