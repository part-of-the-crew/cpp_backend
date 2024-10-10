#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>


using namespace std;

const size_t MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);

        //calculating tf
        for (auto &word : words){
            documents_[word].ids[document_id] += 1.0/words.size();
        }

        document_count_++;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const QueryContent query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        partial_sort(matched_documents.begin(), 
                    matched_documents.begin() + min(matched_documents.size(), MAX_RESULT_DOCUMENT_COUNT), 
                    matched_documents.end(),
                    [](const Document& lhs, const Document& rhs) {
                        return lhs.relevance > rhs.relevance;
                    });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

    void CalculateIdf(void) {
        for (auto &document : documents_){
            document.second.idf = log(document_count_ / (double)document.second.ids.size());
        }
    }
private:

    struct QueryContent {
        map<string, int> query; //value - repeatition number
        set<string> minus;
    };

    struct StringContent {
        double idf;             //inverse document frequency
        map <int, double> ids;
        //   id,  tf            //term frequency
    };

    map<string, StringContent> documents_;

    set<string> stop_words_;

    int document_count_ = 0;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    QueryContent ParseQuery(const string& text) const {
        QueryContent query_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            if (word[0] == '-')
                query_words.minus.insert(word.substr(1));
            else
                query_words.query[word]++;
        }
        return query_words;
    }

    vector<Document> FindAllDocuments(const QueryContent& query_words) const {
        map<int, double> id_rev;
        //  id,  relevance
        vector<Document> matched_documents;
        for ( auto& [word, stringContent] : documents_) {
            auto it = query_words.query.find(word);
            if (it != query_words.query.end()) {
                for (const auto &id: stringContent.ids) {
                    id_rev[id.first] += id.second * stringContent.idf * it->second;
                }
            }
        }
        
        for ( auto& [word , stringContent] : documents_) {
            if (query_words.minus.count(word)){
                for (const auto &id: stringContent.ids) {
                    id_rev[id.first] = -1.0;
                }
            }
        }
        
        //from id_wnumber to matched_documents
        for ( auto const& [id, relevance]: id_rev) {
            if (relevance < 0.0) {
                continue;
            }
            Document document;
            document.id = id;
            document.relevance = relevance;
            matched_documents.push_back(document);
        }
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }
    search_server.CalculateIdf();
    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s     << relevance   << " }"s << endl;
    }
}
