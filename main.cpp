#include <iostream>

#include "globals.hpp"

int main() {
    TextureManager manager("assets/sheet.amap");
    sf::Sprite sprite = manager.get_texture("pawn");

    // test for valid chess.png file

    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML works!");

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }

    return 0;
}