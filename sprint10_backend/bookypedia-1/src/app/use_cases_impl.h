#pragma once
#include <string>
#include <vector>

#include "../domain/unit_of_work.h"
#include "use_cases.h"

namespace app {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::UnitOfWorkFactory uow_factory)
        : uow_factory_{std::move(uow_factory)} {}

    void AddAuthor(const std::string& name) override;
    std::vector<AuthorInfo> ShowAuthors() override;
    void AddBook(const std::string& title, const std::string& author_id, int publication_year) override;
    std::vector<BookInfo> ShowBooks() override;
    std::vector<BookInfo> ShowAuthorBooks(const std::string& author_id) override;

private:
    domain::UnitOfWorkFactory uow_factory_;
};

}  // namespace app
