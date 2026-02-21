// use_cases.h
#pragma once
#include <string>
#include <vector>

namespace app {

struct AuthorInfo {
    std::string id;
    std::string name;
};

struct BookInfo {
    std::string id;
    std::string title;
    int publication_year;
};

struct BookInfoExtra {
    std::string id;
    std::string title;
    std::string author_name;
    int publication_year;
    std::vector<std::string> tags;
};

struct BookAuthorInfo {
    std::string id;  // Add this
    std::string title;
    int publication_year;
    std::string name;  // Author name
};

class UseCases {
public:
    virtual void AddAuthor(const std::string& name) = 0;
    virtual void AddBook(const std::string& title, const std::string& author_id, int publication_year) = 0;

    // Return the specific struct instead of a generic pair
    virtual std::vector<AuthorInfo> ShowAuthors() = 0;
    virtual std::vector<BookAuthorInfo> ShowBooks() = 0;
    virtual std::vector<BookInfo> ShowAuthorBooks(const std::string& author_id) = 0;
    virtual AuthorInfo ShowAuthor(const std::string& author_id) = 0;
    virtual void DeleteAuthor(const std::string& author_id) = 0;
    virtual void EditAuthor(const std::string& author_id, const std::string& new_name) = 0;
    virtual void DeleteBook(const std::string& book_id) = 0;
    virtual void EditBook(const std::string& book_id, const std::string& title, int publication_year,
        const std::vector<std::string>& tags) = 0;

protected:
    ~UseCases() = default;
};

}  // namespace app