#pragma once
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

struct CommandDescription {
    // Определяет, задана ли команда (поле command непустое)
    explicit operator bool() const {
        return !command.empty();
    }

    bool operator!() const {
        return !operator bool();
    }

    std::string command;      // Название команды
    std::string id;           // id маршрута или остановки
    std::string description;  // Параметры команды

    // Перегруженные операторы для удобного использования
    bool operator<(const CommandDescription& other){
        return command < other.command;
    }
    bool operator==(const CommandDescription& other){
        return command == other.command;
    }
};

class InputReader {
public:
    /**
     * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
     */
    void ReadInput(std::istream& in);
    void ParseLine(std::string_view line);
    /**
     * Наполняет данными транспортный справочник, используя команды из commands_
     */
    void ApplyCommands(TransportCatalogue& catalogue);

    void PrintCommands() const;

private:
    std::vector<CommandDescription> commands_;

    void ReorderCommands ( void );
};

namespace parsing { 
CommandDescription ParseCommandDescription(std::string_view line);
}