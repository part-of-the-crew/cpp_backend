#pragma once
#include <string>
#include <vector>

#include "../domain/author_fwd.h"
#include "../domain/book_fwd.h"
#include "use_cases.h"

namespace app {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::AuthorRepository& authors, domain::BookRepository& books)
        : authors_{authors}, books_{books} {}

    void AddAuthor(const std::string& name) override;
    std::vector<AuthorInfo> ShowAuthors(void) override;
    void AddBook(const std::string& title, const std::string& author_id, int publication_year) override;
    std::vector<BookInfo> ShowBooks() override;
    std::vector<BookInfo> ShowAuthorBooks(const std::string& author_id) override;

private:
    domain::AuthorRepository& authors_;
    domain::BookRepository& books_;
};

}  // namespace app
