#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>


#include "json_reader.h"


int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
     */

    json::Document input_doc = json::Load(std::cin);
    TransportCatalogue cat = json_reader::TransformFromJson(input_doc);
    //TransportCatalogue::Requests requests = TransportCatalogue::CalculateRequests(cat);
    //json::Document output_doc = TransformIntoJson(requests);
    //json::Print(output_doc, std::cout);
}