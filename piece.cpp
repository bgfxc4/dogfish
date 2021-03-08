#include <SFML/Graphics.hpp>

#include "enums.hpp"
#include "graphics.hpp"
#include "piece.hpp"

Figure::Figure(Pieces type) {
	this->type = type;

	if (type == Pieces::WhitePawn)
		sprite = new sf::Sprite(figuresTextures[0]);
	else if (type == Pieces::BlackPawn)
		sprite = new sf::Sprite(figuresTextures[6]);
	else if (type == Pieces::WhiteBishop)
		sprite = new sf::Sprite(figuresTextures[1]);
	else if (type == Pieces::BlackBishop)
		sprite = new sf::Sprite(figuresTextures[7]);
	else if (type == Pieces::WhiteKnight)
		sprite = new sf::Sprite(figuresTextures[2]);
	else if (type == Pieces::BlackKnight)
		sprite = new sf::Sprite(figuresTextures[8]);
	else if (type == Pieces::WhiteRook)
		sprite = new sf::Sprite(figuresTextures[3]);
	else if (type == Pieces::BlackRook)
		sprite = new sf::Sprite(figuresTextures[9]);
	else if (type == Pieces::WhiteQueen)
		sprite = new sf::Sprite(figuresTextures[4]);
	else if (type == Pieces::BlackQueen)
		sprite = new sf::Sprite(figuresTextures[10]);
	else if (type == Pieces::WhiteKing)
		sprite = new sf::Sprite(figuresTextures[5]);
	else if (type == Pieces::BlackKing)
		sprite = new sf::Sprite(figuresTextures[11]);
}
Figure::~Figure() {
	if (sprite != nullptr)
		delete sprite;
}

Color Figure::getColor() {
	if (type >= Pieces::BlackPawn && type <= Pieces::BlackQueen) {
		return Color::Black;
	} else if (type != Pieces::Empty) {
		return Color::White;
	} else {
		return Color::NoColor;
	}
}
