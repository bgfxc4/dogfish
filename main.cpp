#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <array>
#include <cctype>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>
#include <iostream>
#include <ctype.h>

#include "graphics.hpp"
#include "board.hpp"

void testing() {
}

int main() {
	testing();

	sf::RenderWindow& window = startGraphics();
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
				if ((sf::Mouse::Button)event.key.code == sf::Mouse::Left)
					board.startMouseClick(sf::Mouse::getPosition(window));
			} else if (event.type == sf::Event::MouseButtonReleased) {
				if ((sf::Mouse::Button)event.key.code == sf::Mouse::Left)
					board.endMouseClick(sf::Mouse::getPosition(window));
			}
		}
        window.clear();
        window.draw(boardSprite);
		board.renderBoard(window);
		window.display();
    }
	delete &window;
	return 0;
}
