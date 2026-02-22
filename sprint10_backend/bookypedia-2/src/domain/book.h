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
    // Добавлен аргумент tags со значением по умолчанию
    Book(BookId id, std::string name, std::string author_id, int publication_year,
        std::vector<std::string> tags = {})
        : id_(std::move(id))
        , name_(std::move(name))
        , author_id_(std::move(author_id))
        , publication_year_(publication_year)
        , tags_(std::move(tags)) {}

    const BookId& GetId() const noexcept { return id_; }

    const std::string& GetName() const noexcept { return name_; }
    const std::string& GetAuthorId() const noexcept { return author_id_; }
    int GetPublicationYear() const noexcept { return publication_year_; }

    // Геттер для тегов
    const std::vector<std::string>& GetTags() const noexcept { return tags_; }

private:
    BookId id_;
    std::string name_;
    std::string author_id_;
    int publication_year_;
    std::vector<std::string> tags_;
};

class BookRepository {
public:
    virtual void Save(const Book& book) = 0;
    virtual std::vector<Book> RetrieveAllBooks(void) = 0;
    virtual std::vector<Book> RetrieveAuthorBooks(const std::string& author_id) = 0;

    virtual Book RetrieveBook(const std::string& book_id) = 0;
    virtual void Delete(const std::string& book_id) = 0;
    virtual bool Update(const std::string& book_id, const std::string& title, int publication_year,
        const std::vector<std::string>& tags) = 0;

protected:
    ~BookRepository() = default;
};

}  // namespace domain