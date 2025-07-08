#include "json_builder.h"
#include <iostream>


using namespace std;

int main() {
    int nTest;
    std::cin >> nTest;
    if(nTest == 1){
        json::Print(
            json::Document{
                json::Builder{}
                .Value("just a string"s)
                .Build()
            },
            cout
        );
        cout << endl;
        return 1;
    }
    if(nTest == 2){
        json::Print(
            json::Document{
                json::Builder{}
                    .StartArray()
                        .Value(456)
                        .Value("fyra, fem, sex"s)
                    .EndArray()
                .Build()
            },
            cout
        );
        cout << endl;
        return 1;
    }
    if(nTest == 3){
        json::Print(
            json::Document{
                json::Builder{}
                    .StartArray()
                        .Value(123)
                        .Value("en, tva, tre"s)
                        .StartArray()
                            .Value(456)
                            .Value("fyra, fem, sex"s)
                        .EndArray()
                    .EndArray()
                .Build()
            },
            cout
        );
        cout << endl;
        return 1;
    }
    if(nTest == 4){
        json::Print(
            json::Document{
                json::Builder{}
                .StartDict()
                    .Key("key1"s).Value(123)
                    .Key("key2"s).Value("value2"s)
                    .Key("key3"s).StartArray()
                        .Value(456)
                        .StartDict().EndDict()
                        .StartDict()
                            .Key(""s)
                            .Value(nullptr)
                        .EndDict()
                        .Value(""s)
                    .EndArray()
                .EndDict()
                .Build()
            },
            cout
        );
        cout << endl;
        return 1;
    }

}