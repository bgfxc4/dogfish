#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>

//0-5 white, 6-11 black, 1. pawn, 2. bishop, 3. knight, 4. rook, 5. queen, 6.king
sf::Texture figuresTextures[12];
sf::Sprite figuresSprites[12];
sf::Sprite boardSprite;
sf::Texture boardTexture;

void load_figure_textures() {
	figuresTextures[0].loadFromFile("images/figures/pawn_white.png");
	figuresSprites[0].setTexture(figuresTextures[0]);
	figuresTextures[1].loadFromFile("images/figures/bishop_white.png");
	figuresSprites[1].setTexture(figuresTextures[1]);
	figuresTextures[2].loadFromFile("images/figures/knight_white.png");
	figuresSprites[2].setTexture(figuresTextures[2]);
	figuresTextures[3].loadFromFile("images/figures/rook_white.png");
	figuresSprites[3].setTexture(figuresTextures[3]);
	figuresTextures[4].loadFromFile("images/figures/queen_white.png");
	figuresSprites[4].setTexture(figuresTextures[4]);
	figuresTextures[5].loadFromFile("images/figures/king_white.png");
	figuresSprites[5].setTexture(figuresTextures[5]);
	figuresTextures[6].loadFromFile("images/figures/pawn_black.png");
	figuresSprites[6].setTexture(figuresTextures[6]);
	figuresTextures[7].loadFromFile("images/figures/bishop_black.png");
	figuresSprites[7].setTexture(figuresTextures[7]);
	figuresTextures[8].loadFromFile("images/figures/knight_black.png");
	figuresSprites[8].setTexture(figuresTextures[8]);
	figuresTextures[9].loadFromFile("images/figures/rook_black.png");
	figuresSprites[9].setTexture(figuresTextures[9]);
	figuresTextures[10].loadFromFile("images/figures/queen_black.png");
	figuresSprites[10].setTexture(figuresTextures[10]);
	figuresTextures[11].loadFromFile("images/figures/king_black.png");
	figuresSprites[11].setTexture(figuresTextures[11]);
}

sf::RenderWindow& startGraphics() {
	sf::RenderWindow& window = *new sf::RenderWindow(sf::VideoMode(700, 700), "chess", sf::Style::Resize);
	window.setFramerateLimit(30);

	boardTexture.loadFromFile("images/board.png");
	boardSprite.setTexture(boardTexture);

	load_figure_textures();
	return window;
}
