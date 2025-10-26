#pragma once

#include <string>
#include <vector>
#include <deque>

#include "search_server.h"
#include "document.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server): search_server_(search_server)  {};

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    
    int GetNoResultRequests() const;
    
private:
    struct QueryResult {
        std::vector<Document> v_docs;
        int isEmpty;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    // возможно, здесь вам понадобится что-то ещё
    const SearchServer& search_server_;
};

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    // напишите реализацию
    const auto query = search_server_.FindTopDocuments<DocumentPredicate>(raw_query, document_predicate);
    if (requests_.size() == min_in_day_)
        requests_.pop_front();
    requests_.push_back({query, query.empty()});
    return query;
};