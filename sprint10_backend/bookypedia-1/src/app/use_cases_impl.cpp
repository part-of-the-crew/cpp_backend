#include "use_cases_impl.h"

#include <stdexcept>
#include <utility>

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string& name) {
    if (name.empty()) throw std::runtime_error("empty name");

    auto uow = uow_factory_();
    uow->Authors().Save({AuthorId::New(), name});
    uow->Commit();
}

std::vector<AuthorInfo> UseCasesImpl::ShowAuthors() {
    auto uow = uow_factory_();
    auto authors = uow->Authors().Retrieve();
    // Read-only, so no Commit() needed

    std::vector<AuthorInfo> result;
    for (const auto& author : authors) {
        result.push_back({author.GetId().ToString(), author.GetName()});
    }
    return result;
}

void UseCasesImpl::AddBook(const std::string& title, const std::string& author_id, int publication_year) {
    if (title.empty()) throw std::runtime_error("empty title");
    if (author_id.empty()) throw std::runtime_error("empty author_id");
    if (publication_year == 0) throw std::runtime_error("empty publication_year");

    auto uow = uow_factory_();
    uow->Books().Save({BookId::New(), title, author_id, publication_year});
    uow->Commit();
}

std::vector<BookInfo> UseCasesImpl::ShowBooks() {
    auto uow = uow_factory_();
    auto books = uow->Books().RetrieveAllBooks();

    std::vector<BookInfo> result;
    for (const auto& book : books) {
        result.push_back({book.GetName(), book.GetAuthorId(), book.GetPublicationYear()});
    }
    return result;
}

std::vector<BookInfo> UseCasesImpl::ShowAuthorBooks(const std::string& author_id) {
    auto uow = uow_factory_();
    auto books = uow->Books().RetrieveAuthorBooks(author_id);

    std::vector<BookInfo> result;
    for (const auto& book : books) {
        result.push_back({book.GetName(), book.GetAuthorId(), book.GetPublicationYear()});
    }
    return result;
}

}  // namespace app