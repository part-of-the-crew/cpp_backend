#include "json_loader.h"

#include <boost/json.hpp>
#include <fstream>
#include <stdexcept>

using namespace std::string_literals;

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
    std::ifstream in(json_path);
    if (!in) {
        throw std::runtime_error("Failed to open file");
    }
    // Распарсить строку как JSON, используя boost::json::parse
    auto content =
        boost::json::parse(std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()));

    // Загрузить модель игры из файла
    model::Game game;
    for (auto& map_json : content.as_object().at("maps"s).as_array()) {
        // content
        auto description = map_json.as_object();
        model::Map map(model::Map::Id{std::string(description.at("id"s).as_string())},
                       std::string(description.at("name"s).as_string()));

        for (const auto& e : description.at("buildings"s).as_array()) {
            const auto& obj = e.as_object();
            model::Point p(model::Coord(obj.at("x"s).as_int64()), model::Coord(obj.at("y"s).as_int64()));
            model::Size s(model::Coord(obj.at("w"s).as_int64()), model::Coord(obj.at("h"s).as_int64()));
            model::Building building(model::Rectangle(p, s));
            map.AddBuilding(building);
        }

        for (const auto& e : description.at("offices"s).as_array()) {
            const auto& obj = e.as_object();
            model::Office::Id id(std::string(obj.at("id"s).as_string()));
            model::Point pos(model::Coord(obj.at("x"s).as_int64()), model::Coord(obj.at("y"s).as_int64()));
            model::Offset s(model::Coord(obj.at("offsetX"s).as_int64()), model::Coord(obj.at("offsetY"s).as_int64()));
            model::Office office(id, pos, s);
            map.AddOffice(std::move(office));
        }

        for (const auto& e : description.at("roads"s).as_array()) {
            const auto& obj = e.as_object();
            model::Point start(model::Coord(obj.at("x0"s).as_int64()), model::Coord(obj.at("y0"s).as_int64()));
            if (auto it = obj.find("x1"s); it != obj.end()) {
                map.AddRoad({model::Road::HORIZONTAL, start, model::Coord(it->value().as_int64())});
            } else {
                map.AddRoad({model::Road::VERTICAL, start, model::Coord(obj.at("y1"s).as_int64())});
            }
        }

        game.AddMap(std::move(map));
    }
    return game;
}

}  // namespace json_loader
