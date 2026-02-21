#pragma once
#include <functional>
#include <memory>
#include "author.h"
#include "book.h"

namespace domain {

class UnitOfWork {
public:
    virtual void Commit() = 0;
    virtual AuthorRepository& Authors() = 0;
    virtual BookRepository& Books() = 0;
    virtual ~UnitOfWork() = default;
};

// Factory to create a new Unit of Work (starts a transaction)
using UnitOfWorkFactory = std::function<std::unique_ptr<UnitOfWork>()>;

} // namespace domain