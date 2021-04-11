#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

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

	if (opt.depth == -1) {
		opt.depth = 5;
	}

	if (opt.threads < 1) error("The engine needs at least one thread to work!", 1);
	if (opt.depth < 1) error("The engine can at least search to depth one!", 1);

	sf::RenderWindow& window = startGraphics();
	Board board;
	BoardUI boardUI = (opt.engine) ? BoardUI(opt.threads, opt.depth, 1) : BoardUI(opt.threads, opt.depth);

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
