#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "constants.hpp"

//0-5 white, 6-11 black, 1. pawn, 2. bishop, 3. knight, 4. rook, 5. queen, 6.king
sf::Texture figuresTextures[13];
sf::Sprite figuresSprites[13];
sf::Sprite boardSprite;
sf::Texture boardTexture;

void load_figure_textures() {
	figuresTextures[(int)Pieces::Pawn].loadFromFile("images/figures/pawn_white.png");
	figuresSprites[(int)Pieces::Pawn].setTexture(figuresTextures[(int)Pieces::Pawn]);
	figuresTextures[(int)Pieces::Bishop].loadFromFile("images/figures/bishop_white.png");
	figuresSprites[(int)Pieces::Bishop].setTexture(figuresTextures[(int)Pieces::Bishop]);
	figuresTextures[(int)Pieces::Knight].loadFromFile("images/figures/knight_white.png");
	figuresSprites[(int)Pieces::Knight].setTexture(figuresTextures[(int)Pieces::Knight]);
	figuresTextures[(int)Pieces::Rook].loadFromFile("images/figures/rook_white.png");
	figuresSprites[(int)Pieces::Rook].setTexture(figuresTextures[(int)Pieces::Rook]);
	figuresTextures[(int)Pieces::Queen].loadFromFile("images/figures/queen_white.png");
	figuresSprites[(int)Pieces::Queen].setTexture(figuresTextures[(int)Pieces::Queen]);
	figuresTextures[(int)Pieces::King].loadFromFile("images/figures/king_white.png");
	figuresSprites[(int)Pieces::King].setTexture(figuresTextures[(int)Pieces::King]);
	figuresTextures[(int)Pieces::Pawn + 6].loadFromFile("images/figures/pawn_black.png");
	figuresSprites[(int)Pieces::Pawn + 6].setTexture(figuresTextures[(int)Pieces::Pawn + 6]);
	figuresTextures[(int)Pieces::Bishop + 6].loadFromFile("images/figures/bishop_black.png");
	figuresSprites[(int)Pieces::Bishop + 6].setTexture(figuresTextures[(int)Pieces::Bishop + 6]);
	figuresTextures[(int)Pieces::Knight + 6].loadFromFile("images/figures/knight_black.png");
	figuresSprites[(int)Pieces::Knight + 6].setTexture(figuresTextures[(int)Pieces::Knight + 6]);
	figuresTextures[(int)Pieces::Rook + 6].loadFromFile("images/figures/rook_black.png");
	figuresSprites[(int)Pieces::Rook + 6].setTexture(figuresTextures[(int)Pieces::Rook + 6]);
	figuresTextures[(int)Pieces::Queen + 6].loadFromFile("images/figures/queen_black.png");
	figuresSprites[(int)Pieces::Queen + 6].setTexture(figuresTextures[(int)Pieces::Queen + 6]);
	figuresTextures[(int)Pieces::King + 6].loadFromFile("images/figures/king_black.png");
	figuresSprites[(int)Pieces::King + 6].setTexture(figuresTextures[(int)Pieces::King + 6]);
}

sf::RenderWindow& startGraphics() {
	sf::RenderWindow& window = *new sf::RenderWindow(sf::VideoMode(700, 700), "chess", sf::Style::Resize);
	window.setFramerateLimit(30);

	boardTexture.loadFromFile("images/board.png");
	boardSprite.setTexture(boardTexture);

	load_figure_textures();
	return window;
}
