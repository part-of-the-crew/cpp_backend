#include "bookypedia.h"

#include <iostream>

#include "app/use_cases_impl.h"
#include "domain/unit_of_work.h"
#include "menu/menu.h"
#include "postgres/postgres.h"
#include "ui/view.h"

namespace bookypedia {

using namespace std::literals;
Application::Application(const AppConfig& config)
    : db_{pqxx::connection{config.db_url}}
    , use_cases_([this]() { return std::make_unique<postgres::UnitOfWorkImpl>(db_.GetConnection()); }) {}

void Application::Run() {
    menu::Menu menu{std::cin, std::cout};
    menu.AddAction("Help"s, {}, "Show instructions"s, [&menu](std::istream&) {
        menu.ShowInstructions();
        return true;
    });
    menu.AddAction("Exit"s, {}, "Exit program"s, [&menu](std::istream&) { return false; });
    ui::View view{menu, use_cases_, std::cin, std::cout};
    menu.Run();
}

}  // namespace bookypedia
