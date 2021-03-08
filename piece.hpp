#pragma once

#include <SFML/Graphics.hpp>

#include "enums.hpp"

class Figure {
	public:
	sf::Sprite* sprite;
	Pieces type;

	bool isDragged = false;

	Figure(Pieces);
	~Figure();

	Color getColor();
};
