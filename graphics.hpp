#pragma once

#include <SFML/Graphics.hpp>

extern sf::Texture figuresTextures[12];
extern sf::Texture boardTexture;
extern sf::Sprite boardSprite;

void load_figure_textures();
sf::RenderWindow& startGraphics();
