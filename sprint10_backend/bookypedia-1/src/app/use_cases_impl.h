#pragma once
#include <string>
#include <vector>

#include "../domain/author_fwd.h"
#include "use_cases.h"

namespace app {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::AuthorRepository& authors) : authors_{authors} {}

    void AddAuthor(const std::string& name) override;
    std::vector<std::pair<std::string, std::string>> ShowAuthors(void) override;
    void AddBook(const std::string& title, const std::string& author_id, int publication_year) override;
    void ShowBooks() override;
    void ShowAuthorBooks(const std::string& author_id) override;

private:
    domain::AuthorRepository& authors_;
};

}  // namespace app
