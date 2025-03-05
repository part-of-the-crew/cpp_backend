#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"
#include <cassert>
using namespace std;

int main() {
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

    statistics::ReadAndPrintRequests(catalogue, cin, cout);
}