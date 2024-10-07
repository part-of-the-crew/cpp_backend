#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;
/*

    push(x) — добавить число x в очередь;

    pop() — удалить число из очереди и вывести на печать;

    peek() — напечатать первое число в очереди;

    size() — вернуть размер очереди;



*/
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

set<string> ParseStopWords(const string text) {
    set<string> stop_words;
    for (const string& word : SplitIntoWords(text)) {
        stop_words.insert(word);
    }
    return stop_words;
}

vector<string> SplitIntoWordsNoStop(const string& text, const set<string>& stop_words) {
    vector<string> words;
    // проходим по всем словам из текста и проверяем, что их нет в списке стоп-слов
    for (const string& word : SplitIntoWords(text)) {
        if (stop_words.count(word) == 0) {
            words.push_back(word);
        }
    }
    // вернём результат без стоп-слов
    return words;
}

void AddDocument(vector<vector<string>>& documents, const set<string>& stop_words,
                 const string& document) {
    const vector<string> words = SplitIntoWordsNoStop(document, stop_words);
    documents.push_back(words);
}

// Разбирает text на слова и возвращает только те из них, которые не входят в stop_words
set<string> ParseQuery(const string& text, const set<string>& stop_words) {
    vector<string> vquery_words = SplitIntoWordsNoStop(text, stop_words);
    set<string>  query_words(vquery_words.cbegin(), vquery_words.cend());
    return query_words;
}

// Возвращает true, если среди слов документа (document_words)
// встречаются слова поискового запроса query_words
bool MatchDocument(const vector<string>& document_words, const set<string>& query_words) {
    // Напишите код функции
    for (const string& word : document_words) {
        if (query_words.count(word) == 1) {
            return true;
        }
    }
    return false;
}

// Возвращает массив id документов, подходящих под запрос query
// Стоп-слова исключаются из поиска
vector<int> FindDocuments(const vector<vector<string>>& documents, const set<string>& stop_words,
                          string query) {
    vector<int> matched_documents;
    set<string> query_words = ParseQuery(query, stop_words);
    
    //stop_words = stop_words;
    // Напишите код функции
    // Воспользуйте вспомогательными функциями ParseQuery, MatchDocument
    // В качестве id документа используйте его индекс в массиве documents
    for (long unsigned int i = 0; i < documents.size(); ++i) 
        if (MatchDocument(documents[i], query_words))
            matched_documents.push_back(i);
    return matched_documents;
}

int main() {
    const set<string> stop_words = ParseStopWords(ReadLine());

    // Read documents
    vector<vector<string>> documents;
    const int document_count = ReadLineWithNumber();
    for (int i = 0; i < document_count; ++i) {
        AddDocument(documents, stop_words, ReadLine());
    }

    string query = ReadLine();
    for (const int i : FindDocuments(documents, stop_words, query)) {
        cout << "{ document_id = "s << i << " }"s << endl;
    }
}
