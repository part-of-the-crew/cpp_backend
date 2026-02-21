#pragma once
#include <string>
#include <vector>

#include "../util/tagged_uuid.h"

namespace domain {

namespace detail {
struct BookTag {};
}  // namespace detail

using BookId = util::TaggedUUID<detail::BookTag>;

class Book {
public:
    Book(BookId id, std::string name, std::string author_id, int publication_year)
        : id_(std::move(id))
        , name_(std::move(name))
        , author_id_(std::move(author_id))
        , publication_year_(publication_year) {}

    const BookId& GetId() const noexcept { return id_; }

    const std::string& GetName() const noexcept { return name_; }
    const std::string& GetAuthorId() const noexcept { return author_id_; }
    int GetPublicationYear() const noexcept { return publication_year_; }

private:
    BookId id_;
    std::string name_;
    std::string author_id_;
    int publication_year_;
};

class BookRepository {
public:
    virtual void Save(const Book& Book) = 0;
    virtual std::vector<Book> RetrieveAllBooks(void) = 0;
    virtual std::vector<Book> RetrieveAuthorBooks(const std::string& author_id) = 0;

protected:
    ~BookRepository() = default;
};

}  // namespace domain
