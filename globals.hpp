#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <optional>
#include <cassert>
#include <filesystem>

enum class Piece {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

std::unordered_map<Piece, sf::IntRect> text_coords_by_piece{
    {Piece::PAWN, sf::IntRect({8, 16}, {16, 32})},
    {Piece::KNIGHT, sf::IntRect({40, 16}, {20, 32})},
    {Piece::BISHOP, sf::IntRect({72, 12}, {16, 36})},
    {Piece::ROOK, sf::IntRect({102, 10}, {20, 38})},
    {Piece::QUEEN, sf::IntRect({134, 0}, {20, 48})},
    {Piece::KING, sf::IntRect({166, 2}, {20, 46})},
};

bool ends_with(const std::string& main, const std::string& match) {
    if (main.length() >= match.length()) {
        return (main.compare(main.length() - match.length(), match.length(), match) == 0);
    }
    else {
        return false;
    }
}

std::vector<std::string> read_file(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream input_file(filename);

    if (!input_file.is_open()) {
        throw std::runtime_error("");
    }

    std::string line;
    while (std::getline(input_file, line)) {
        lines.push_back(line);
    }

    input_file.close();
    return lines;
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> result;
    size_t pos = s.find(delimiter);

    if (pos == std::string::npos) { // Delimiter not found
        result.push_back(s);
    }
    else {
        result.push_back(s.substr(0, pos)); // Part before the delimiter
        result.push_back(s.substr(pos + 1)); // Part after the delimiter
    }
    return result;
}

struct NamedSprite {
    std::string name;
    sf::Sprite sprite;
};


class TextureManager {
private:
    sf::Texture spritesheet;
    std::vector<NamedSprite> sprites;

public:
    TextureManager(const std::string& filename) {
        //assert(ends_with(filename, ".amap"), "wrong filename");
        std::vector<std::string> lines = read_file(filename);

        sf::IntRect location;
        std::string name;
        std::string spritesheet_filename;

        // resolve .amap directory so relative paths inside the file work
        std::filesystem::path amap_path = std::filesystem::absolute(filename);
        std::filesystem::path amap_dir = amap_path.parent_path();

        for (size_t i = 0; i < lines.size(); i++) {
            std::vector<std::string> parts = split(lines[i], ':');
            if (parts.empty()) continue;

            if (parts[0] == "path") {
                // path in the .amap may be relative to the .amap file
                std::string provided = parts.size() > 1 ? parts[1] : std::string();
                std::filesystem::path sheet_path = amap_dir / provided;
                spritesheet_filename = sheet_path.string();
                if (!spritesheet.loadFromFile(spritesheet_filename)) {
                    throw std::runtime_error(std::string("Failed to load image\n    Provided path: ") + provided +
                        "\n    Absolute path: " + spritesheet_filename + "\nReason: No such file or directory");
                }
                continue;
            }
            else if (parts[0] == "asset") {
                name = parts[1];
                if (i + 3 < lines.size()) {
                    for (int j = 1; j <= 2; j++) {
                        parts = split(lines[i + j], ':');
                        std::string key = parts[0];
                        std::string value = parts[1];
                        parts = split(value, ',');
                        sf::Vector2i nums = { std::stoi(parts[0]), std::stoi(parts[1]) };
                        if (key == "start") {
                            location.position = nums;
                        }
                        else if (key == "size") {
                            location.size = nums;
                        }
                    }
                    i += 2;
                    NamedSprite named_sprite{name, sf::Sprite(spritesheet, location)};
                    sprites.push_back(std::move(named_sprite));
                }
                continue;
            }
        }
    }

    const sf::Sprite& get_texture(const std::string& name) {
        for (const NamedSprite& ns : sprites) {
            if (ns.name == name) {
                return ns.sprite;
            }
        }
        throw std::runtime_error("Texture not found: " + name);
    }
};