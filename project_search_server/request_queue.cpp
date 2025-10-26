#include "request_queue.h"

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    // напишите реализацию
    const auto query = search_server_.FindTopDocuments(raw_query, status);
    if (requests_.size() == min_in_day_)
        requests_.pop_front();
    requests_.push_back({query, query.empty()});
    return query;
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    // напишите реализацию
    const auto query = search_server_.FindTopDocuments(raw_query);
    if (requests_.size() == min_in_day_)
        requests_.pop_front();
    requests_.push_back({query, query.empty()});
    return query;
}

int RequestQueue::GetNoResultRequests() const {
    // напишите реализацию
    int ret = 0;
    for (auto const &e: requests_){
        if(e.isEmpty)
            ret++;
    }
    return ret;
}