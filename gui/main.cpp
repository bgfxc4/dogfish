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
#include "board-ui.hpp"
#include "parse_opts.hpp"

void testing() {
}

int main(int argc, char* argv[]) {
	testing();
	
	struct opts opt;
	parse_opts(argc, argv, &opt);

	if (opt.help) return 0;
	
	if (opt.threads == -1) {
		opt.threads = std::thread::hardware_concurrency();
	}	

	if (opt.engine && opt.threads < 3) error("If -e or --engine is provided, -t or --threads needs to be at least 3 (1 for UI, 1 for the managing part of the engine, 1 for the calculating part of the engine))!", 1);

	sf::RenderWindow& window = startGraphics();
	Board board;
	BoardUI boardUI = (opt.engine) ? BoardUI(opt.threads, 1) : BoardUI(opt.threads);

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
					boardUI.startMouseClick(sf::Mouse::getPosition(window), board);
			} else if (event.type == sf::Event::MouseButtonReleased) {
				if ((sf::Mouse::Button)event.key.code == sf::Mouse::Left)
					boardUI.endMouseClick(sf::Mouse::getPosition(window), board);
			}
		}
		window.clear();
		window.draw(boardSprite);
		boardUI.renderBoard(window, board);
		window.display();
	}
	delete &window;
	return 0;
}
