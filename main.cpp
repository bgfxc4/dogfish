#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cctype>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>
#include <iostream>
#include <ctype.h>

enum class Pieces { 
	Empty,
	WhitePawn,
	WhiteBishop,
	WhiteKnight,
	WhiteRook,
	WhiteKing,
	WhiteQueen,
	BlackPawn,
	BlackBishop,
	BlackKnight, 
	BlackRook,
	BlackKing,
	BlackQueen 
};
enum class Color { 
	White, 
	Black,
	NoColor
};

//0-5 white, 6-11 black, 1. pawn, 2. bishop, 3. knight, 4. rook, 5. queen, 6.king
sf::Texture figuresTextures[12];


int* tileNameToPosition(std::string tileName) {
	static int ret[2] = { -1, -1 };

	if (tileName.size() != 2) {
		std::cout << "[ERROR] tileName must be 2 characters long!" << std::endl;
		return ret;
	}
	ret[0] = tileName[0] - 'a';
	ret[1] = '8' - tileName[1];

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
	sf::Sprite* sprite;
	Pieces type = Pieces::Empty;

	bool isDragged = false;

	Figure(Pieces type) {
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
	~Figure() {
		if (sprite != nullptr)
			delete sprite;
	}

	Color getColor() {
		if (type == Pieces::WhitePawn || type == Pieces::WhiteBishop || type == Pieces::WhiteKnight || type == Pieces::WhiteRook || type == Pieces::WhiteQueen || type == Pieces::WhiteKing) {
			return Color::White;
		} else if (type != Pieces::Empty) {
			return Color::Black;
		} else {
			return Color::NoColor;
		}
	}

};

class Tile {
	public:
	sf::Vector2i position;
	std::array<std::array<Tile*, 8>, 8>* tiles;
	std::vector<int[2]> possibleMoves;
	bool enPassantPossible = false;
	Figure* figure;

	Tile(int x, int y, std::array<std::array<Tile*, 8>, 8>* tiles) {
		position.x = x;
		position.y = y;
		this->tiles = tiles;
		this->figure = nullptr;
	}
	~Tile() {
		if (figure != nullptr)
			delete figure;
	}

	void setFigure(Figure* toSet) {
		figure = toSet;
	}

	void move(Tile* startTile, Tile* endTile) {
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

	std::vector<sf::Vector2i> calculateAllMoves() {
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
						if (tiles->at(position.x - 1).at(position.y - 1)->figure->getColor() != Color::White) {
							possibleMoves.push_back(sf::Vector2i(position.x - 1, position.y - 1));
						}
					} else if (tiles->at(position.x - 1).at(position.y - 1)->enPassantPossible)
						possibleMoves.push_back(sf::Vector2i(position.x - 1, position.y - 1));
				}
				if (position.x != 7) { //not completly right
					if (tiles->at(position.x + 1).at(position.y - 1)->figure != nullptr) {
						if (tiles->at(position.x + 1).at(position.y - 1)->figure->getColor() != Color::White) {
							possibleMoves.push_back(sf::Vector2i(position.x + 1, position.y - 1));
						}
					} else if (tiles->at(position.x + 1).at(position.y - 1)->enPassantPossible)
						possibleMoves.push_back(sf::Vector2i(position.x + 1, position.y - 1));
				}
			}
		}
		return possibleMoves;
	}

	void renderFigure(sf::RenderWindow& window) {
		if (!figure->isDragged) 
			figure->sprite->setPosition(position.x * 75, position.y * 75);
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
	std::array<int, 2> selectedTile = { -1, -1 };
	Color toMove = Color::White;
	int halfMoves = 0; //moves in a row, where no pawn was moved or no pice was taken
	int moveCount = 1; //starts at 1, +1 after black moved
	bool whiteCanCastleLong = false;
	bool whiteCanCastleShot = false;
	bool blackCanCastleLong = false;
	bool blackCanCastleShot = false;
	
	sf::Texture selectedTileWhiteTexture = sf::Texture();
	sf::Texture selectedTileBlackTexture = sf::Texture();
	sf::Texture possibleMoveTexture = sf::Texture();
	sf::Texture possibleTakeTexture = sf::Texture();
	sf::Sprite selectedTileWhiteSprite;
	sf::Sprite selectedTileBlackSprite;
	sf::Sprite possibleMoveSprite;
	sf::Sprite possibleTakeSprite;

	Figure* draggedFigure = nullptr;
	sf::Vector2i dragStartPos;

	Board() {
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
	~Board() {
		for (std::array<Tile*, 8>& vec : tiles) {
			for (Tile* tile : vec) {
				delete tile->figure;
				delete tile;
			}
		}
	}

	void renderBoard(sf::RenderWindow& window) {
		renderSelectedTiles(window);
		renderFigures(window);
		if (selectedTile[0] != -1 && selectedTile[1] != -1)
			renderPossibleMoves(window);
	}
	
	void renderSelectedTiles(sf::RenderWindow& window) {
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
	
	void renderPossibleMoves(sf::RenderWindow& window) {
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

	void renderFigures(sf::RenderWindow& window) {
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

	void move(int startX, int startY, int endX, int endY) {
		if (startX == endX && startY == endY)
			return;

		Tile* startTile = tiles[startX][startY];
		Tile* endTile = tiles[endX][endY];

		if (endTile->enPassantPossible) { //en Passant happened
			std::cout << "en passant" << std::endl;
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
	
	void startMouseClick(sf::Vector2i mousePos) {
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

	void endMouseClick(sf::Vector2i mousePos) {		
		if (draggedFigure == nullptr)
			return;

		draggedFigure->isDragged = false;
		draggedFigure = nullptr;
		
		if ((mousePos.x / 75) >= (int)tiles.size() || (mousePos.y / 75) >= (int)tiles[0].size()) 
			return;
		move(dragStartPos.x / 75, dragStartPos.y / 75, mousePos.x / 75, mousePos.y / 75);
	}

	int parseFEN(std::string fenString) {
		int group = 1;
		int row = 0, col = 0;

		char temp_char = 0;
		std::string tempStr5thGroup = "";
		std::string tempStr6thGroup = "";

		for (char c : fenString) {
			if (group == 1) {
				Pieces type = Pieces::Empty;
				if (c == '/') {
					row++;
					col = 0;
				} else if (std::isdigit(c)) 
					col += c - '0';
				else if (c == 'R' || c == 'r') 
					type = (c == 'R') ? Pieces::WhiteRook : Pieces::BlackRook;
				else if (c == 'N' || c == 'n') 
					type = (c == 'N') ? Pieces::WhiteKnight : Pieces::BlackKnight;
				else if (c == 'B' || c == 'b') 
					type = (c == 'B') ? Pieces::WhiteBishop : Pieces::BlackBishop;
				else if (c == 'Q' || c == 'q') 
					type = (c == 'Q') ? Pieces::WhiteQueen :Pieces::BlackQueen;
				else if (c == 'K' || c == 'k') 
					type = (c == 'K') ? Pieces::WhiteKing : Pieces::BlackKing;
				else if (c == 'P' || c == 'p') 
					type = (c == 'P') ? Pieces::WhitePawn : Pieces::BlackPawn;
				else if (c == ' ')
					group ++;
				else {
					std::cout << "[ERROR] invalid fen string at 1st group c: " << c << std::endl;
					return -1;
				}
				
				if (type != Pieces::Empty) {
					Figure* figure = new Figure(type);
					tiles[col][row]->setFigure(figure);
					col++;
				}

			} else if (group == 2) {
				if (c == 'w') 
					toMove = Color::White;
				else if (c == 'b') 
					toMove = Color::Black;
				else if (c == ' ') 
					group++;
				else {
					std::cout << "[ERROR] invalid fen string at 2nd group" << std::endl;
					return -1;
				}
			} else if (group == 3) {
				if (c == 'Q') 
					whiteCanCastleLong = true;
				else if (c == 'K') 
					whiteCanCastleShot = true;
				else if (c == 'q') 
					blackCanCastleLong = true;
				else if (c == 'k') 
					blackCanCastleShot = true;
				else if (c == ' ') 
					group++;
				else {
					std::cout << "[ERROR] invalid fen string at 3rd group" << std::endl;
					return -1;
				}
			} else if (group == 4) {
				if (c == '-') 
					continue;
				else if (c == ' ') 
					group++;
				else if (!std::isdigit(c))
					temp_char = c;
				else if (std::isdigit(c)) {
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
					group++;	
				} else if (std::isdigit(c)) 
					tempStr5thGroup += c; 
				else {
					std::cout << "[ERROR] invalid fen string at 5th group" << std::endl;
					return -1;
				}
			} else if (group == 6) {
				if (c == ' ') {
					moveCount = std::stoi(tempStr6thGroup);
					return 0;	
				} else if (std::isdigit(c)) 
					tempStr6thGroup += c; 
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

void testing() {
}

int main() {
	testing();

	sf::RenderWindow window(sf::VideoMode(700, 700), "Cheess!", sf::Style::Resize);
	window.setFramerateLimit(30);

	sf::Texture boardTexture;
	boardTexture.loadFromFile("images/board.png");
	sf::Sprite boardSprite(boardTexture);

	load_figure_textures();

	Board board;
	
    while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) 
				window.close();
			else if (event.type == sf::Event::Resized) {
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			} else if (event.type == sf::Event::MouseButtonPressed) {
				if (event.key.code == sf::Mouse::Left) 
					board.startMouseClick(sf::Mouse::getPosition(window));
			} else if (event.type == sf::Event::MouseButtonReleased) {
				if (event.key.code == sf::Mouse::Left) 
					board.endMouseClick(sf::Mouse::getPosition(window));
			}
		}
        window.clear();
        window.draw(boardSprite);
		board.renderBoard(window);
		window.display();
    }
	return 0;
}
