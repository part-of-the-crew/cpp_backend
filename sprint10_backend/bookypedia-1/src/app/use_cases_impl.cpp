#include "use_cases_impl.h"

#include <stdexcept>
#include <utility>

#include "../domain/author.h"
#include "../domain/book.h"

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string& name) {
    if (name.empty())
        throw std::runtime_error("empty name");
    authors_.Save({AuthorId::New(), name});
}

std::vector<AuthorInfo> UseCasesImpl::ShowAuthors(void) {
    std::vector<AuthorInfo> result;
    // Retrieve returns domain objects, we map them to app DTOs
    for (const auto& author : authors_.Retrieve()) {
        result.push_back({author.GetId().ToString(), author.GetName()});
    }
    return result;
}

void UseCasesImpl::AddBook(const std::string& title, const std::string& author_id, int publication_year) {
    if (title.empty())
        throw std::runtime_error("empty title");
    if (author_id.empty())
        throw std::runtime_error("empty author_id");
    if (publication_year == 0)
        throw std::runtime_error("empty publication_year");

    books_.Save({BookId::New(), title, author_id, publication_year});
}

std::vector<BookInfo> UseCasesImpl::ShowBooks() {
    std::vector<BookInfo> books;
    for (const auto& book : books_.RetrieveAllBooks()) {
        books.push_back({book.GetName(), book.GetAuthorId(), book.GetPublicationYear()});
    }
    return books;
}

std::vector<BookInfo> UseCasesImpl::ShowAuthorBooks(const std::string& author_id) {
    std::vector<BookInfo> books;
    for (const auto& book : books_.RetrieveAuthorBooks(author_id)) {
        books.push_back({book.GetName(), book.GetAuthorId(), book.GetPublicationYear()});
    }
    return books;
}

}  // namespace app
