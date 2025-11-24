#include "multi_colored_text.hpp"

MultiColoredText::MultiColoredText(sf::Font& font, std::string text, unsigned int character_size)
	: font(&font), text(text), character_size(character_size), texture(&font.getTexture(character_size)), full_text(font.getTexture(character_size)) {

	sf::Vector2u new_size = calculate_size();
    render_texture = sf::RenderTexture(new_size);

    burn_text_to_sprite();
}

void MultiColoredText::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(full_text, states);
}

void MultiColoredText::set_color(sf::Color color, std::optional<Slice> slice_opt) {
    if (slice_opt.has_value()) {
        Slice s = slice_opt.value();
        s.start = std::min(s.start, text.size());
        s.length = std::min(s.length, text.size() - s.start);

        color_map[s] = color;
    }
    else {
        Slice full{ 0, text.size() };
        color_map[full] = color;
    }

    burn_text_to_sprite();
}


std::string MultiColoredText::get_string() const {
	return text;
}

void MultiColoredText::set_string(std::string string) {
	text = string;

    sf::Vector2u new_size = calculate_size();
    render_texture = sf::RenderTexture(new_size);

    burn_text_to_sprite();
}

unsigned int MultiColoredText::get_character_size() const {
	return character_size;
}

void MultiColoredText::set_character_size(unsigned int size) {
	character_size = size;
	texture = &font->getTexture(character_size);

    sf::Vector2u new_size = calculate_size();
    render_texture = sf::RenderTexture(new_size);

    burn_text_to_sprite();
}

sf::Font* MultiColoredText::get_font() const {
	return font;
}

void MultiColoredText::set_font(sf::Font& font) {
	this->font = &font;

    sf::Vector2u new_size = calculate_size();
    render_texture = sf::RenderTexture(new_size);

    burn_text_to_sprite();
}

sf::Vector2f MultiColoredText::get_position() const {
	return position;
}

void MultiColoredText::set_position(sf::Vector2f position) {
	this->position = position;
	full_text.setPosition(position);
}

sf::Vector2u MultiColoredText::calculate_size() const {
    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float max_y = std::numeric_limits<float>::lowest();

    sf::Vector2f pen = { 0.f, 0.f };
    const float line_spacing = font->getLineSpacing(character_size);

    for (std::size_t i = 0; i < text.size(); ++i)
    {
        char c = text[i];

        if (c == '\n')
        {
            pen.x = 0.f;
            pen.y += line_spacing;
            continue;
        }

        const sf::Glyph& glyph = font->getGlyph(c, character_size, false);

        // The exact same transform you use when drawing:
        float draw_x = pen.x + glyph.bounds.position.x;
        float draw_y = pen.y + glyph.bounds.position.y + character_size;

        float w = glyph.bounds.size.x;
        float h = glyph.bounds.size.y;

        // Track bounds
        min_x = std::min(min_x, draw_x);
        min_y = std::min(min_y, draw_y);
        max_x = std::max(max_x, draw_x + w);
        max_y = std::max(max_y, draw_y + h);

        pen.x += glyph.advance;
    }

    // No glyphs? Return empty
    if (min_x == std::numeric_limits<float>::max())
        return { 0, 0 };

    return sf::Vector2u(
        static_cast<unsigned>(max_x - min_x),
        static_cast<unsigned>(max_y - min_y)
    );
}

void MultiColoredText::burn_text_to_sprite() {
    sf::Vector2f pen_position = position;
    const float line_spacing = font->getLineSpacing(character_size);

    Slice full_slice{ 0, text.size() };

    for (std::size_t i = 0; i < text.size(); i++) {
        char c = text[i];

        if (c == '\n') {
            pen_position.x = position.x;
            pen_position.y += line_spacing;
            continue;
        }

        const sf::Glyph& glyph = font->getGlyph(c, character_size, false);

        sf::Color color = sf::Color::White;

        for (const auto& [slice, col] : color_map) {
            if (i >= slice.start && i < slice.start + slice.length) {
                color = col;
                break;
            }
        }

        sf::Sprite sprite(*texture);
        sprite.setTextureRect(sf::IntRect(glyph.textureRect));
        sprite.setColor(color);

        float baseline_y = pen_position.y + glyph.bounds.position.y;
        sprite.setPosition({ pen_position.x, baseline_y });

		render_texture.draw(sprite);

        pen_position.x += glyph.advance;
    }

    render_texture.display();
	full_text.setTexture(render_texture.getTexture());
}