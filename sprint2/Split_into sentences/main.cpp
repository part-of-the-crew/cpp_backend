#include <algorithm>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <utility>
using namespace std;


template <typename Token>
using Sentence = vector<Token>;

template <typename TokenForwardIt>
TokenForwardIt FindSentenceEnd(TokenForwardIt tokens_begin, TokenForwardIt tokens_end) {
    const TokenForwardIt before_sentence_end
        = adjacent_find(tokens_begin, tokens_end, [](const auto& left_token, const auto& right_token) {
              return left_token.IsEndSentencePunctuation() && !right_token.IsEndSentencePunctuation();
          });
    return before_sentence_end == tokens_end ? tokens_end : next(before_sentence_end);
}

// Класс Token имеет метод bool IsEndSentencePunctuation() const
template <typename Token>
vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens) {
    vector<Sentence<Token>> sentences;
    auto current_sentence_start = tokens.begin();
    auto tokens_end = tokens.end();

    while (current_sentence_start != tokens_end) {
        auto sentence_end = FindSentenceEnd(current_sentence_start, tokens_end);

        // Collect the current sentence
        Sentence<Token> sentence(make_move_iterator(current_sentence_start),
                                                make_move_iterator(sentence_end));

        sentences.push_back(std::move(sentence));

        if (sentence_end != tokens_end) {
            current_sentence_start = next(sentence_end);
            --current_sentence_start;
        } else {
            break;
        }
    }

    return sentences;
}

// Класс Token имеет метод bool IsEndSentencePunctuation() const
template <typename Token>
vector<Sentence<Token>> SplitIntoSentences4(vector<Token> tokens) {
    vector<Sentence<Token>> sentences;
    auto tokens_begin = begin(tokens);
    const auto tokens_end = end(tokens);
    while (tokens_begin != tokens_end) {
        const auto sentence_end = FindSentenceEnd(tokens_begin, tokens_end);
        sentences.push_back(Sentence<Token>(make_move_iterator(tokens_begin), make_move_iterator(sentence_end)));
        tokens_begin = sentence_end;
    }
    return sentences;
}


struct TestToken {
    string data;
    bool is_end_sentence_punctuation = false;

    bool IsEndSentencePunctuation() const {
        return is_end_sentence_punctuation;
    }
    bool operator==(const TestToken& other) const {
        return data == other.data && is_end_sentence_punctuation == other.is_end_sentence_punctuation;
    }
};

ostream& operator<<(ostream& stream, const TestToken& token) {
    return stream << token.data;
}

// Тест содержит копирования объектов класса TestToken.
// Для проверки отсутствия копирований в функции SplitIntoSentences
// необходимо написать отдельный тест.
void TestSplitting() {

    auto v = SplitIntoSentences(vector<TestToken>(
               {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}, {"!"s, true}, {"Without"s}, {"copies"s}, {"."s, true}}));
    //print vector of TestToken
    /*
    for (const auto& sentence : v) {
        for (const auto& token : sentence) {
            cout << token.data << " ";
        }
        cout << endl;
    }*/
    assert(SplitIntoSentences(vector<TestToken>({{"Split"s}, {"into"s}, {"sentences"s}, {"!"s}}))
           == vector<Sentence<TestToken>>({{{"Split"s}, {"into"s}, {"sentences"s}, {"!"s}}}));

    assert(SplitIntoSentences(vector<TestToken>({{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}}))
           == vector<Sentence<TestToken>>({{{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}}}));

    assert(SplitIntoSentences(vector<TestToken>(
               {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}, {"!"s, true}, {"Without"s}, {"copies"s}, {"."s, true}}))
           == vector<Sentence<TestToken>>({
               {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}, {"!"s, true}},
               {{"Without"s}, {"copies"s}, {"."s, true}},
           }));
    
}

struct TestNoCopyToken : TestToken {
    TestNoCopyToken(const TestNoCopyToken&) = delete;
    TestNoCopyToken(TestNoCopyToken&&) = default;
};

void TestNoCopy() {
    vector<TestNoCopyToken> tokens;
    tokens.push_back({"Split"s});
    tokens.push_back({"!"s, true});
    tokens.push_back({"Move"s});
    tokens.push_back({"!"s, true});
    const auto sentences = SplitIntoSentences(move(tokens));
    assert(sentences[0][0] == TestNoCopyToken({"Split"s}));
    assert(sentences[0][1] == TestNoCopyToken({"!"s, true}));
    assert(sentences[1][0] == TestNoCopyToken({"Move"s}));
    assert(sentences[1][1] == TestNoCopyToken({"!"s, true}));
}

int main() {
    TestSplitting();
    TestNoCopy();
}