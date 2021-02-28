#include <SFML/Graphics.hpp>

//0-5 white, 6-11 black, 1. pawn, 2. bishop, 3. knight, 4. rook, 5. queen, 6.king
sf::Texture figuresTextures[12];
sf::Sprite boardSprite;
sf::Texture boardTexture;

void load_figure_textures() {
	figuresTextures[0].loadFromFile("images/figures/pawn_white.png");
	figuresTextures[1].loadFromFile("images/figures/bishop_white.png");
	figuresTextures[2].loadFromFile("images/figures/knight_white.png");
	figuresTextures[3].loadFromFile("images/figures/rook_white.png");
	figuresTextures[4].loadFromFile("images/figures/queen_white.png");
	figuresTextures[5].loadFromFile("images/figures/king_white.png");
	figuresTextures[6].loadFromFile("images/figures/pawn_black.png");
	figuresTextures[7].loadFromFile("images/figures/bishop_black.png");
	figuresTextures[8].loadFromFile("images/figures/knight_black.png");
	figuresTextures[9].loadFromFile("images/figures/rook_black.png");
	figuresTextures[10].loadFromFile("images/figures/queen_black.png");
	figuresTextures[11].loadFromFile("images/figures/king_black.png");
}

sf::RenderWindow& startGraphics() {
	sf::RenderWindow& window = *new sf::RenderWindow(sf::VideoMode(700, 700), "Cheess!", sf::Style::Resize);
	window.setFramerateLimit(30);

	boardTexture.loadFromFile("images/board.png");
	boardSprite.setTexture(boardTexture);

	load_figure_textures();
	return window;
}
