#include "use_cases_impl.h"

#include <stdexcept>
#include <utility>

#include "../domain/author.h"

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string& name) {
    if (name.empty())
        throw std::runtime_error("empty name");
    authors_.Save({AuthorId::New(), name});
}

std::vector<std::pair<std::string, std::string>> UseCasesImpl::ShowAuthors(void) {
    std::vector<std::pair<std::string, std::string>> authors;
    for (const auto& author : authors_.Retrieve()) {
        authors.push_back(std::make_pair(author.GetId().ToString(), author.GetName()));
    }
    return authors;
}

void UseCasesImpl::AddBook(const std::string& title, const std::string& author_id, int publication_year) {
    if (title.empty())
        throw std::runtime_error("empty title");
    if (author_id.empty())
        throw std::runtime_error("empty author_id");
    if (publication_year == 0)
        throw std::runtime_error("empty publication_year");
    /*
    authors_.Save({AuthorId::New(), name});
    for (const auto& author : authors_.Retrieve()) {
        if (author.GetId().ToString() == author_id)
    }
        */
}

void UseCasesImpl::ShowBooks() {
    std::vector<std::pair<std::string, std::string>> books;
    /*
    for (const auto& author : authors_.Retrieve()) {
        for (const auto& book : author.GetBooks()) {
            books.push_back(std::make_pair(book.GetTitle(), book.GetPublicationYear()));
        }
    }
        */
}

void UseCasesImpl::ShowAuthorBooks(const std::string& author_id) {
    /*   std::vector<std::pair<std::string, std::string>> books;
    for (const auto& author : authors_.Retrieve()) {
        if (author.GetId().ToString() == author_id) {
            for (const auto& book : author.GetBooks())
        }
    }
        */
}

}  // namespace app
