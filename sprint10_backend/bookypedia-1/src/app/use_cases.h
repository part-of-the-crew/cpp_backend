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
    std::string title;
    std::string author_id;  // Or std::string author_name if that's what the UI needs
    int publication_year;
};

class UseCases {
public:
    virtual void AddAuthor(const std::string& name) = 0;
    virtual void AddBook(const std::string& title, const std::string& author_id, int publication_year) = 0;

    // Return the specific struct instead of a generic pair
    virtual std::vector<AuthorInfo> ShowAuthors() = 0;
    virtual std::vector<BookInfo> ShowBooks() = 0;
    virtual std::vector<BookInfo> ShowAuthorBooks(const std::string& author_id) = 0;

protected:
    ~UseCases() = default;
};

}  // namespace app