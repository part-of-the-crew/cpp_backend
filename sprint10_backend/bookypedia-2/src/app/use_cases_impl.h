#pragma once
#include <string>
#include <vector>

#include "../domain/unit_of_work.h"
#include "use_cases.h"

namespace app {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::UnitOfWorkFactory uow_factory) : uow_factory_{std::move(uow_factory)} {}

    void AddAuthor(const std::string& name) override;
    std::vector<AuthorInfo> ShowAuthors() override;
    void AddBook(const std::string& title, const std::string& author_id, int publication_year) override;
    std::vector<BookAuthorInfo> ShowBooks() override;
    std::vector<BookInfo> ShowAuthorBooks(const std::string& author_id) override;
    AuthorInfo ShowAuthor(const std::string& author_id) override;
    void DeleteAuthor(const std::string& author_id) override;
    void EditAuthor(const std::string& author_id, const std::string& new_name) override;
    void DeleteBook(const std::string& book_id) override;
    void EditBook(const std::string& book_id, const std::string& title, int publication_year,
        const std::vector<std::string>& tags) override;

private:
    domain::UnitOfWorkFactory uow_factory_;
};

}  // namespace app
