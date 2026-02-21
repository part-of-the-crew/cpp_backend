#include "use_cases_impl.h"

#include <stdexcept>
// #include <utility>

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string& name) {
    if (name.empty())
        throw std::runtime_error("empty name");

    auto uow = uow_factory_();
    uow->Authors().Save({AuthorId::New(), name});
    uow->Commit();
}

void UseCasesImpl::DeleteAuthor(const std::string& author_id) {
    auto uow = uow_factory_();
    uow->Authors().Delete(author_id);
    uow->Commit();
}

void UseCasesImpl::EditAuthor(const std::string& author_id, const std::string& new_name) {
    if (new_name.empty())
        throw std::runtime_error("empty name");

    auto uow = uow_factory_();
    auto author = uow->Authors().RetrieveAuthor(author_id);
    uow->Authors().Save({author.GetId(), new_name});
    uow->Commit();
}

std::vector<AuthorInfo> UseCasesImpl::ShowAuthors() {
    auto uow = uow_factory_();
    auto authors = uow->Authors().Retrieve();

    std::vector<AuthorInfo> result;
    for (const auto& author : authors) {
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

    auto uow = uow_factory_();
    uow->Books().Save({BookId::New(), title, author_id, publication_year});
    uow->Commit();
}

AuthorInfo UseCasesImpl::ShowAuthor(const std::string& author_id) {
    auto uow = uow_factory_();
    auto author = uow->Authors().RetrieveAuthor(author_id);

    return {author.GetId().ToString(), author.GetName()};
}

std::vector<BookAuthorInfo> UseCasesImpl::ShowBooks() {
    auto uow = uow_factory_();
    auto books = uow->Books().RetrieveAllBooks();

    std::vector<BookAuthorInfo> result;
    for (const auto& book : books) {
        auto author = uow->Authors().RetrieveAuthor(book.GetAuthorId());
        result.push_back(
            {book.GetId().ToString(), book.GetName(), book.GetPublicationYear(), author.GetName()});
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

void UseCasesImpl::DeleteBook(const std::string& book_id) {
    auto uow = uow_factory_();
    uow->Books().Delete(book_id);
    uow->Commit();
}

void UseCasesImpl::EditBook(const std::string& book_id, const std::string& title, int publication_year,
    const std::vector<std::string>& tags) {
    auto uow = uow_factory_();

    // Pass the update down to the repository
    bool success = uow->Books().Update(book_id, title, publication_year, tags);
    if (!success) {
        throw std::invalid_argument("Book not found");  // Catches concurrent deletions
    }

    uow->Commit();
}

}  // namespace app