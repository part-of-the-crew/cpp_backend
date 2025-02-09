#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"
#include <cassert>
using namespace std;

int main() {
    /*
    {
        TransportCatalogue catalogue;
        catalogue.AddStop({"Moscow", {0.0, 0.0}});
        Stop s1 ("Moscow", {0.0, 0.0});
        assert((catalogue.stops.back() == s1));

        assert((catalogue.stopname_to_stop.find("Moscow") != catalogue.stopname_to_stop.cend()));

        catalogue.AddStop({"Spb", {1.0, 2.0}});
        assert((catalogue.stops.back() == Stop("Spb", {1.0, 2.0})));
        assert((catalogue.stopname_to_stop.find("Moscow") != catalogue.stopname_to_stop.cend()));
        assert((catalogue.stopname_to_stop.find("Spb") != catalogue.stopname_to_stop.cend()));

        catalogue.AddRoute("750", {"Moscow", "Spb"});
        assert((catalogue.routes.back().name == "750"));
        assert((catalogue.routes.back().stops == 
                std::vector<std::deque<Stop>::const_iterator>({catalogue.stopname_to_stop.find("Moscow")->second,
                               catalogue.stopname_to_stop.find("Spb")->second})));

                                       catalogue.AddRoute("751", {"Spb", "Moscow"});
        assert((catalogue.routes.back().name == "751"));
        assert((catalogue.routes.back().stops == 
                std::vector<std::deque<Stop>::const_iterator>({catalogue.stopname_to_stop.find("Spb")->second,
                               catalogue.stopname_to_stop.find("Moscow")->second})));
    }
                               */
    {
        std::string s1 = "Bus 11";
        assert("11" == statistics::ParseRequest(s1).id);
        assert("Bus" == statistics::ParseRequest(s1).command);
        std::string s2 = "Bus  12";

        assert("12" == statistics::ParseRequest(s2).id);
        std::string s3 = "Bus  13  ";
        //std::cout << ParseBusName(s3) << std::endl;
        //return 0;
        //assert("13" == ParseBusName(s3));
        std::string s4 = " Bus  14  ";
        //assert(("14" == ParseBusName(s4)));
        std::string s5 = " Bus  15 67";
        //std::cout << ParseBusName(s5) << std::endl;
        //return 0;
        assert(("15 67" == statistics::ParseRequest(s5).id));
        assert("Bus" == statistics::ParseRequest(s5).command);
    }
    //return 0;
    TransportCatalogue catalogue;
    {
        InputReader reader;
        reader.ReadInput(cin);
        reader.ApplyCommands(catalogue);
    }

    int stat_request_count;
    cin >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(cin, line);
        statistics::ParseAndPrint(catalogue, line, cout);
    }
}