#pragma once
#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <optional>

struct Slice {
	size_t start;
	size_t length;
};

struct SliceHash {
	size_t operator()(Slice s) const {
		return std::hash<size_t>{}(s.start) ^ (std::hash<size_t>{}(s.length) << 1);
	}
};

struct SliceEq {
	bool operator()(Slice a, Slice b) const {
		return a.start == b.start && a.length == b.length;
	}
};

class MultiColoredText {
public:
	MultiColoredText(sf::Font& font, std::string text, unsigned int character_size = 30U);
	
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	void set_color(sf::Color color, std::optional<Slice> slice_opt = std::nullopt);

	// boilerplate functions
	std::string get_string() const;

	void set_string(std::string string);

	unsigned int get_character_size() const;

	void set_character_size(unsigned int size);

	sf::Font* get_font() const;

	void set_font(sf::Font& font);

	sf::Vector2f get_position() const;

	void set_position(sf::Vector2f position);

private:

	sf::Vector2u calculate_size() const;

	void burn_text_to_sprite();

	std::unordered_map<Slice, sf::Color, SliceHash, SliceEq> color_map;

	sf::Sprite full_text;
	sf::RenderTexture render_texture;
	std::string text;
	sf::Font* font;
	unsigned int character_size = 30U;
	sf::Vector2f position;
	const sf::Texture* texture;
};