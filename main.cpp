#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cctype>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>
#include <iostream>
#include <ctype.h>

enum class Pieces { Empty, WhitePawn, WhiteBishop, WiteKnight, WhiteRook, WhiteKing, WhiteQueen, BlackPawn, BlackBishop, BlackKnight, BlackRook, BlackKing, BlackQueen };
enum class Color { White, Black};

//0-5 white, 6-11 black, 1. pawn, 2. bishop, 3. knight, 4. rook, 5. queen, 6.king
sf::Texture figuresTextures[12];


int* tileNameToPosition(std::string tileName) {
	static int ret[2] = { -1, -1};

	if (tileName.size() != 2) {
		std::cout << "[ERROR] tileName must be 2 characters long!" << std::endl;
		return ret;
	}
	if (tileName[0] == 'a') ret[0] = 0;
	else if (tileName[0] == 'b') ret[0] = 1;
	else if (tileName[0] == 'c') ret[0] = 2;
	else if (tileName[0] == 'd') ret[0] = 3;
	else if (tileName[0] == 'e') ret[0] = 4;
	else if (tileName[0] == 'f') ret[0] = 5;
	else if (tileName[0] == 'g') ret[0] = 6;
	else if (tileName[0] == 'h') ret[0] = 7;

	if (tileName[1] == '1') ret[1] = 7;
	else if (tileName[1] == '2') ret[1] = 6;
	else if (tileName[1] == '3') ret[1] = 5;
	else if (tileName[1] == '4') ret[1] = 4;
	else if (tileName[1] == '5') ret[1] = 3;
	else if (tileName[1] == '6') ret[1] = 2;
	else if (tileName[1] == '7') ret[1] = 1;
	else if (tileName[1] == '8') ret[1] = 0;
	return ret;
}

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


class Figure {
	public:
	bool isDragged = false;

	Figure(sf::Texture& texture) {
		sprite = new sf::Sprite(texture);
	}
	~Figure() {
		delete sprite;
	}
	
	sf::Sprite* sprite;
	Pieces piece = Pieces::Empty;
};

class Tile {
	public:
	int position[2];
	std::vector<int[2]> possibleMoves;
	bool enPassantPossible = false;
	Figure* figure;

	Tile(int x, int y) {
		position[0] = x;
		position[1] = y;
		this->figure = nullptr;
	}
	~Tile() {
		delete figure;
	}

	void setFigure(Figure* toSet) {
		figure = toSet;
	}

	void calculateAllMoves() {
		//TODO
	}

	void renderFigure(sf::RenderWindow& window) {
		if (!figure->isDragged) figure->sprite->setPosition(position[0] * 75, position[1] * 75);
		else {
			sf::Vector2i mousePos = sf::Mouse::getPosition(window);
			figure->sprite->setPosition(mousePos.x - 37.5f, mousePos.y - 37.5f);
		} 
		window.draw(*(figure->sprite));
	}
};

class Board {
	public:
	std::array<std::array<Tile*, 8>, 8> tiles;
	Color toMove = Color::White;
	int halfMoves = 0; //moves in a row, where no pawn was moved or no pice was taken
	int moveCount = 1; //starts at 1, +1 after black moved
	bool whiteCanCastleLong = false;
	bool whiteCanCastleShot = false;
	bool blackCanCastleLong = false;
	bool blackCanCastleShot = false;

	Figure* draggedFigure = nullptr;
	sf::Vector2i dragStartPos;

	Board() {
		for (int i = 0; i < 8; i ++) {
			for (int j = 0; j < 8; j ++) {
				tiles[i][j] = new Tile(i, j);
			}
		}
		parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		std::cout << halfMoves << " " << moveCount << std::endl;
	}
	~Board() {
		for (std::array<Tile*, 8>& vec : tiles) {
			for (Tile* tile : vec) {
				delete tile->figure;
				delete tile;
			}
		}
	}

	void renderBoard(sf::RenderWindow& window) {
		for (int x = 0; x < (int)tiles.size(); x ++) {
			for (int y = 0; y < (int)tiles[x].size(); y ++) {
				if (tiles[x][y]->figure == nullptr) continue;
				tiles[x][y]->renderFigure(window);
			}
		}
	}

	void move(int startX, int startY, int endX, int endY) {
		if (startX == endX && startY == endY) return;
		delete tiles[endX][endY]->figure;
		tiles[endX][endY]->setFigure(tiles[startX][startY]->figure);
		tiles[startX][startY]->figure = nullptr;
	}
	
	void startMouseClick(sf::Vector2i mousePos) {
		if ((mousePos.x / 75) >= (int)tiles.size() || (mousePos.y / 75) >= (int)tiles[0].size()) return;
		if (tiles[mousePos.x / 75][mousePos.y / 75]->figure == nullptr) return;
		draggedFigure = tiles[mousePos.x / 75][mousePos.y / 75]->figure;
		dragStartPos = mousePos;
		draggedFigure->isDragged = true;
	}

	void endMouseClick(sf::Vector2i mousePos) {
		if (draggedFigure == nullptr) return;
		draggedFigure->isDragged = false;
		move(dragStartPos.x / 75, dragStartPos.y / 75, mousePos.x / 75, mousePos.y / 75);
		draggedFigure = nullptr;
	}

	int parseFEN(std::string fenString) {
		int group = 1;
		int row = 0, col = 0;

		char temp_char = 0;
		std::string tempStr5thGroup = "";
		std::string tempStr6thGroup = "";

		for (char c : fenString) {
			if (group == 1) {
				int i = -1;
				if (c == '/') {
					row ++;
					col = 0;
				} else if (std::isdigit(c)) col += c - '0';
				else if (c == 'R' || c == 'r') i = (c == 'R') ? 3 : 9;
				else if (c == 'N' || c == 'n') i = (c == 'N') ? 2 : 8;
				else if (c == 'B' || c == 'b') i = (c == 'B') ? 1 : 7;
				else if (c == 'Q' || c == 'q') i = (c == 'Q') ? 4 : 10;
				else if (c == 'K' || c == 'k') i = (c == 'K') ? 5 : 11;
				else if (c == 'P' || c == 'p') i = (c == 'P') ? 0 : 6;
				else if (c == ' ') {
					group ++;
				} else {
					std::cout << "[ERROR] invalid fen string at 1st group c: " << c << std::endl;
					return -1;
				}
				
				if (i != -1) {
					Figure* figure = new Figure(figuresTextures[i]);
					tiles[col][row]->setFigure(figure);
					col ++;
				}

			} else if (group == 2) {
				if (c == 'w') toMove = Color::White;
				else if (c == 'b') toMove = Color::Black;
				else if (c == ' ') group ++;
				else {
					std::cout << "[ERROR] invalid fen string at 2nd group" << std::endl;
					return -1;
				}
			} else if (group == 3) {
				if (c == 'Q') whiteCanCastleLong = true;
				else if (c == 'K') whiteCanCastleShot = true;
				else if (c == 'q') blackCanCastleLong = true;
				else if (c == 'k') blackCanCastleShot = true;
				else if (c == ' ') group ++;
				else {
					std::cout << "[ERROR] invalid fen string at 3rd group" << std::endl;
					return -1;
				}
			} else if (group == 4) {
				if (c == '-') continue;
				else if (c == ' ') group ++;
				else if (!std::isdigit(c)) {
					temp_char = c;
				} else if (std::isdigit(c)) {
					if (temp_char == 0) {
						std::cout << "[ERROR] invalid fen string at 4th group(1)" << std::endl;
						return -1;
					} else {
						std::string tileName = std::string() + temp_char + c;
						int* enPassantTile = tileNameToPosition(tileName);
						tiles[enPassantTile[0]][enPassantTile[1]]->enPassantPossible = true;
						temp_char = 0;
					}
				} else {
					std::cout << "[ERROR] invalid fen string at 4th group(2)" << std::endl;
					return -1;
				}
			} else if (group == 5) {
				if (c == ' ') {
					halfMoves = std::stoi(tempStr5thGroup);
					group ++;	
				} else if (std::isdigit(c)) tempStr5thGroup += c; 
				else {
					std::cout << "[ERROR] invalid fen string at 5th group" << std::endl;
					return -1;
				}
			} else if (group == 6) {
				if (c == ' ') {
					moveCount = std::stoi(tempStr6thGroup);
					return 0;	
				} else if (std::isdigit(c)) tempStr6thGroup += c; 
				else {
					std::cout << "[ERROR] invalid fen string at 6th group" << std::endl;
					return -1;
				}
		
			}
		}
		moveCount = std::stoi(tempStr6thGroup);
		return 0;
	}

	std::string getFEN() {
		//TODO
		return "";
	}
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(700, 700), "Cheess!", sf::Style::Resize);
	window.setFramerateLimit(30);

	sf::Texture boardTexture;
	boardTexture.loadFromFile("images/board.png");
	sf::Sprite boardSprite(boardTexture);

	load_figure_textures();

	Board board;
	
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();
			else if (event.type == sf::Event::Resized) {
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			} else if (event.type == sf::Event::MouseButtonPressed) {
				if (event.key.code == sf::Mouse::Left) {
					board.startMouseClick(sf::Mouse::getPosition(window));
				}
			} else if (event.type == sf::Event::MouseButtonReleased) {
				if (event.key.code == sf::Mouse::Left) {
					board.endMouseClick(sf::Mouse::getPosition(window));
				}
			}
        }
		
        window.clear();
        window.draw(boardSprite);
		board.renderBoard(window);
		window.display();
    }

    return 0;
}

