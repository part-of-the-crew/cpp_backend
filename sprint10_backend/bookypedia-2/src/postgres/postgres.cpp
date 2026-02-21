#include "postgres.h"
// #include <exception>
#include <pqxx/pqxx>
#include <pqxx/zview.hxx>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void AuthorRepositoryImpl::Save(const domain::Author& author) {
    work_.exec_params(
        R"(
        INSERT INTO authors (id, name) VALUES ($1, $2)
            ON CONFLICT (id) DO UPDATE SET name=$2;
        )"_zv,
        author.GetId().ToString(), author.GetName());
}

void AuthorRepositoryImpl::Delete(const std::string& author_id) {
    // work_.exec_params("DELETE FROM books WHERE author_id = $1"_zv, author_id);

    work_.exec_params("DELETE FROM authors WHERE id = $1"_zv, author_id);
}

std::vector<domain::Author> AuthorRepositoryImpl::Retrieve(void) {
    std::vector<domain::Author> authors;

    const auto request = work_.exec_params(
        R"(SELECT id, name FROM authors 
        ORDER BY name ASC)"_zv);
    for (const auto& row : request) {
        authors.push_back(
            {domain::AuthorId::FromString(row["id"].as<std::string>()), row["name"].as<std::string>()});
    }
    return authors;
}

domain::Author AuthorRepositoryImpl::RetrieveAuthor(const std::string& author_id) {
    const auto request = work_.exec_params(
        R"(SELECT id, name FROM authors 
        WHERE id = $1)"_zv,
        author_id);
    return {domain::AuthorId::FromString(request[0]["id"].as<std::string>()),
        request[0]["name"].as<std::string>()};
}

void BookRepositoryImpl::Save(const domain::Book& book) {
    work_.exec_params(
        R"(
INSERT INTO books (id, title, author_id, publication_year) VALUES ($1, $2, $3, $4)
ON CONFLICT (id) DO UPDATE SET title=$2, author_id=$3, publication_year=$4;
)"_zv,
        book.GetId().ToString(),
        book.GetName(),  // В вашем book.h метод называется GetName(), хотя возвращает title
        book.GetAuthorId(), book.GetPublicationYear());
}

std::vector<domain::Book> BookRepositoryImpl::RetrieveAllBooks(void) {
    std::vector<domain::Book> books;

    // Use JOIN to connect books and authors, then order by title, then author name
    const auto request = work_.exec_params(R"(
        SELECT b.id, b.title, b.author_id, b.publication_year 
        FROM books b
        JOIN authors a ON b.author_id = a.id
        ORDER BY b.title ASC, a.name ASC, b.publication_year DESC
    )"_zv);

    for (const auto& row : request) {
        books.push_back(
            {domain::BookId::FromString(row["id"].as<std::string>()), row["title"].as<std::string>(),
                row["author_id"].as<std::string>(), row["publication_year"].as<int>()});
    }
    return books;
}

std::vector<domain::Book> BookRepositoryImpl::RetrieveAuthorBooks(const std::string& author_id) {
    std::vector<domain::Book> books;

    const auto request = work_.exec_params(
        R"(
            SELECT id, title, author_id, publication_year FROM books 
            WHERE author_id = $1
            ORDER BY publication_year ASC
            )"_zv,
        author_id);

    for (const auto& row : request) {
        books.push_back(
            {domain::BookId::FromString(row["id"].as<std::string>()), row["title"].as<std::string>(),
                row["author_id"].as<std::string>(), row["publication_year"].as<int>()});
    }
    return books;
}

void BookRepositoryImpl::Delete(const std::string& book_id) {
    work_.exec_params("DELETE FROM book_tags WHERE book_id = $1"_zv, book_id);
    work_.exec_params("DELETE FROM books WHERE id = $1"_zv, book_id);
}

bool BookRepositoryImpl::Update(const std::string& book_id, const std::string& title, int publication_year,
    const std::vector<std::string>& tags) {
    // 1. Update the main book table
    auto result = work_.exec_params("UPDATE books SET title = $2, publication_year = $3 WHERE id = $1"_zv,
        book_id, title, publication_year);

    // If no rows were updated, the book was deleted by another instance
    if (result.affected_rows() == 0) {
        return false;
    }

    // 2. Overwrite tags (simplest approach: delete old, insert new)
    work_.exec_params("DELETE FROM book_tags WHERE book_id = $1"_zv, book_id);

    for (const auto& tag : tags) {
        work_.exec_params("INSERT INTO book_tags (book_id, tag) VALUES ($1, $2)"_zv, book_id, tag);
    }

    return true;
}

Database::Database(pqxx::connection connection) : connection_{std::move(connection)} {
    pqxx::work work_{connection_};
    work_.exec(R"(
    CREATE TABLE IF NOT EXISTS authors (
        id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
        name varchar(100) UNIQUE NOT NULL
        );
    )"_zv);

    work_.exec(R"(
CREATE TABLE IF NOT EXISTS books (
    id UUID PRIMARY KEY,
    author_id UUID,
    title VARCHAR(100) NOT NULL,
    publication_year INT,
    CONSTRAINT fk_author
        FOREIGN KEY (author_id) 
        REFERENCES authors(id)
        ON DELETE CASCADE
);
    )"_zv);

    work_.exec(R"(
    CREATE TABLE IF NOT EXISTS book_tags (
    book_id UUID,
    tag VARCHAR(30)
        );
    )"_zv);
    work_.commit();
}

}  // namespace postgres