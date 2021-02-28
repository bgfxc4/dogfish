#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <array>

#include "figure.hpp"

class Tile {
	public:
	sf::Vector2i position;
	std::array<std::array<Tile*, 8>, 8>* tiles;
	std::vector<int[2]> possibleMoves;
	bool enPassantPossible = false;
	Figure* figure;

	Tile(int x, int y, std::array<std::array<Tile*, 8>, 8>* tiles);
	~Tile();

	void setFigure(Figure* toSet);

	void move(Tile* startTile, Tile* endTile);
	std::vector<sf::Vector2i> calculateAllMoves();
	void renderFigure(sf::RenderWindow& window);
};
