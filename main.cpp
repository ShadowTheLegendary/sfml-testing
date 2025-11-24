#include <iostream>

#include "multi_colored_text.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My window");

    sf::Font iosvka;
    if (not iosvka.openFromFile("fonts/Iosevka.ttc")) {
		throw std::runtime_error("Failed to load font");
    }

    MultiColoredText text(iosvka, "Hello SFML!\nLook, a newline!");
    text.set_color(sf::Color::Red, Slice{0, 5});
	text.set_color(sf::Color::Green, Slice{ 6, 4 });
    text.set_color(sf::Color::Cyan, Slice{ 10, 1 });
    text.set_color(sf::Color::Yellow, Slice{ 16, 1 });
    text.set_color(sf::Color::Magenta, Slice{ 20, 7 });
    text.set_position({ 5.f, 5.f });

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

		text.draw(window, sf::RenderStates::Default);

        window.display();
    }
}