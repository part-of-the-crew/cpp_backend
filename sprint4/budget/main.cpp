#include "parser.h"
#include "budget_manager.h"


#include <iostream>
#include <string_view>
#include <string>

using namespace std::literals;

void ParseAndProcessQuery(BudgetManager& manager, std::string_view line, std::ostream& out) {
    auto vec_cmd = SplitIntoWordsView(line);

    if (vec_cmd.at(0) == "Earn"){
        manager.Earn(Date{vec_cmd.at(1)}, Date{vec_cmd.at(2)}, std::stoi(std::string(vec_cmd.at(3))));
        return;
    }
    if (vec_cmd.at(0) == "ComputeIncome"){
        out << manager.ComputeIncome(Date{vec_cmd.at(1)}, Date{vec_cmd.at(2)}) << std::endl;
        return;
    }
    if (vec_cmd.at(0) == "PayTax"){
        manager.PayTax(Date{vec_cmd.at(1)}, Date{vec_cmd.at(2)}, std::stoi(std::string(vec_cmd.at(3))));
        return;
    }
    if (vec_cmd.at(0) == "Spend"){
        manager.Spend(Date{vec_cmd.at(1)}, Date{vec_cmd.at(2)}, std::stoi(std::string(vec_cmd.at(3))));
        return;
    }

}

int main() {


    BudgetManager manager;

    const int query_count = ReadNumberOnLine(std::cin);

    for (int i = 0; i < query_count; ++i) {
        std::string line;
        std::getline(std::cin, line);
        ParseAndProcessQuery(manager, line, std::cout);
    }
}