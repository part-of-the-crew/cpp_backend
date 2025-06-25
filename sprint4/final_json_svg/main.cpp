#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>


#include "json_reader.h"
#include "map_renderer.h"

void some_tests(void){

    ;
}

int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
     */
    some_tests();
    json::Document input_doc = json::Load(std::cin);
    json_reader::JsonReader jreader{input_doc};
    transport_catalogue::TransportCatalogue cat = jreader.CreateTransportCatalogue();
    //std::cout << jreader.stops_.size() << jreader.buses_.begin()->name <<std::endl;
    auto requests{jreader.CalculateRequests(cat)};
    //std::cout << requests.size() << " " << cat.stopname_to_bus.size() << std::endl;
    json::Document output_doc = json_reader::TransformRequestsIntoJson(requests);
    json::Print(output_doc, std::cout);
}