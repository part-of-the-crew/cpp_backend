//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».
/* решение задачи "Выводим результаты поиска страницами" из темы "Итераторы" */


#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include <cmath>
#include <numeric>
#include <functional>


#include "paginator.h"
#include "request_queue.h"

#include "read_input_functions.h"
#include "search_server.h"

using namespace std;


template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const string& t_str, const string& u_str, 
                     const string& file, const string& func, unsigned line, const string& hint) {
    if (t != u) {
        cerr << boolalpha;
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        cerr << t << " != "s << u << "."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

void AssertImpl(bool value, const string& expr_str, const string& file, const string& func, unsigned line,
                const string& hint) {
    if (!value) {
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT("s << expr_str << ") failed."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))

template <typename TestFunc>
void RunTestImpl(const TestFunc& func, const string& test_name) {
    func();
    cerr << test_name << " OK"s << endl;
}

#define RUN_TEST(func) RunTestImpl(func, #func)

// -------- Начало модульных тестов поисковой системы ----------

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 40;
    const string content = "The big dog in a small city"s;
    const vector<int> ratings = {5, 2, 3, 1};
    {
        SearchServer server(""s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("dog"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
        const auto accumulated_rating = accumulate(ratings.begin(), ratings.end(), 0);
        ASSERT_EQUAL(doc0.rating, static_cast<int>(accumulated_rating/ratings.size()));
    }

    {
        SearchServer server("dog a"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("a"s).empty(),
                    "Stop words must be excluded from documents"s);
    }
}

/*
Разместите код остальных тестов здесь
*/

void MinusWordsAreNotParsed() {
    const int doc_id1 = 40;
    const int doc_id2 = 140;
    const string content1 = "The big dog in a small city"s;
    const string content2 = "The big kid in a well-kept village"s;
    const vector<int> ratings1 = {-2, -4, -6, -8};
    const vector<int> ratings2 = {1, 2, 3, 4};
    {
        SearchServer server(""s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("big -well-kept"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id1);
        const auto accumulated_rating = accumulate(ratings1.begin(), ratings1.end(), 0);
        ASSERT_EQUAL(doc0.rating, static_cast<int>(accumulated_rating/ratings1.size()));
        //cout << doc0.relevance << endl;
        //ASSERT_EQUAL(doc0.relevance, accumulated_rating/ratings.size());
    }

    {
        SearchServer server(""s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("big -big"s);
        ASSERT_EQUAL(found_docs.size(), 0u);
    }

    {
        SearchServer server(""s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.GetDocumentCount();
        ASSERT_EQUAL(found_docs, 2);
    }
}

void FilteringNotParsed() {
    const int doc_id1 = 40;
    const int doc_id2 = 140;
    const string content1 = "The big dog in a small city"s;
    const string content2 = "The big kid in a well-kept village"s;
    const vector<int> ratings1 = {-2, -4, -6, -8};
    const vector<int> ratings2 = {1, 2, 3, 4};
    {
        SearchServer server(""s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::BANNED, ratings2);
        const auto found_docs = server.FindTopDocuments("big "s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id1);
    }

    {
        SearchServer server(""s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::BANNED, ratings2);
        const auto found_docs = server.FindTopDocuments("big "s, DocumentStatus::BANNED);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id2);
    }

    {
        auto filter1 = []([[maybe_unused]] int document_id, 
                                      [[maybe_unused]] DocumentStatus status_, [[maybe_unused]] int rating) { 
            return rating < 0; 
        };
        SearchServer server(""s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::BANNED, ratings2);
        const auto found_docs = server.FindTopDocuments("big "s, filter1);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id1);
    }
}

void CalcSortRelevance() {
    {
        SearchServer search_server("и в на"s);
        search_server.AddDocument(0, "белый кот и модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
        search_server.AddDocument(1, "пушистый кот пушистый хвост"s,       DocumentStatus::ACTUAL, {7, 2, 7});
        search_server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, {5, -12, 2, 1});
        search_server.AddDocument(3, "ухоженный скворец евгений"s,         DocumentStatus::BANNED, {9});

        const auto found_docs = search_server.FindTopDocuments("пушистый ухоженный кот"s);
        
        ASSERT_EQUAL(found_docs.size(), 3u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, 1);
        ASSERT(doc0.relevance > 0.866433 && doc0.relevance < 0.866435);
    }
}

void CalcMatched() {
    {
        SearchServer search_server("и в на"s);
        search_server.AddDocument(0, "белый кот и модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
        search_server.AddDocument(1, "пушистый кот пушистый хвост"s,       DocumentStatus::ACTUAL, {7, 2, 7});
        search_server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, {5, -12, 2, 1});
        search_server.AddDocument(3, "ухоженный скворец евгений"s,         DocumentStatus::BANNED, {9});
        DocumentStatus status1, status2, status3;
        std::vector<std::string> words1, words2, words3;
        tie(words1, status1) = search_server.MatchDocument("-пушистый кот"s, 0);
        std::vector<std::string> v1{"кот"};
        ASSERT_EQUAL(words1.size(), v1.size());
        ASSERT_EQUAL(static_cast<int>(status1), 0);

        tie(words2, status2) = search_server.MatchDocument("-пушистый кот"s, 3);
        std::vector<std::string> v2{};
        ASSERT_EQUAL(words2.size(), v2.size());
        ASSERT_EQUAL(static_cast<int>(status2),  2);

        tie(words3, status3) = search_server.MatchDocument("-пушистый пушистый"s, 1);
        std::vector<std::string> v3{};
        ASSERT_EQUAL(words3.size(), v3.size());

        /*
            { document_id = 0, status = 0, words = кот}
            { document_id = 1, status = 0, words = кот пушистый}
            { document_id = 2, status = 0, words =}
            { document_id = 3, status = 2, words =} 
        */
    }
}

// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(MinusWordsAreNotParsed);
    RUN_TEST(FilteringNotParsed);
    RUN_TEST(CalcSortRelevance);
    RUN_TEST(CalcMatched);
}

// --------- Окончание модульных тестов поисковой системы -----------

int main() {
    TestSearchServer();
    // Если вы видите эту строку, значит все тесты прошли успешно
    cout << "Search server testing finished"s << endl;
}
