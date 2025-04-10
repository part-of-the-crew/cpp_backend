#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"
#include <cassert>
#include <vector>
using namespace std;

int main() {
    {
        std::string s1 = "Bus 11";
        assert("11" == statistics::ParseRequest(s1).id);
        assert("Bus" == statistics::ParseRequest(s1).command);
        std::string s2 = "Bus  12";

        assert("12" == statistics::ParseRequest(s2).id);
        std::string s4 = " Bus  14  ";
        //assert(("14" == ParseBusName(s4)));
        std::string s5 = " Bus  15 67";
        //std::cout << ParseBusName(s5) << std::endl;
        //return 0;
        assert(("15 67" == statistics::ParseRequest(s5).id));
        assert("Bus" == statistics::ParseRequest(s5).command);
    }
    {
        auto vp = parsing::ParseDistances("55.611087, 37.20829");
        assert(vp.size() == 0);

    }
    {
        auto vp = parsing::ParseDistances("55.611087, 37.20829, 3900m to Marushkino");
        auto test_vp = std::vector<std::pair<std::string, int>> ({std::make_pair("Marushkino", 3900)});
        assert(vp == test_vp);

    }
    {
        auto vp = parsing::ParseDistances("55.595884, 37.209755, 9900m to Biryulyovo Passazhirskaya1, 100m to Biryulyovo Passazhirskaya2");
        auto test_vp = std::vector<std::pair<std::string, int>> (
            {
                {"Biryulyovo Passazhirskaya1", 9900},
                {"Biryulyovo Passazhirskaya2", 100}
            }
        );
        assert(vp == test_vp);
    }
    //return 0;
    TransportCatalogue catalogue;
    {
        InputReader reader;
        reader.ReadInput(cin);
        reader.ApplyCommands(catalogue);
        catalogue.ReallocateDistances();
    }

    statistics::ReadAndPrintRequests(catalogue, cin, cout);
}