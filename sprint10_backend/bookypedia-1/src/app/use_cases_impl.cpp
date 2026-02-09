#include "use_cases_impl.h"

#include <stdexcept>

#include "../domain/author.h"

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string& name) {
    if (name.empty())
        throw std::runtime_error("empty name");
    authors_.Save({AuthorId::New(), name});
}

}  // namespace app
