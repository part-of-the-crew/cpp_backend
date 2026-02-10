#include "postgres.h"
// #include <exception>
#include <pqxx/pqxx>
#include <pqxx/zview.hxx>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void AuthorRepositoryImpl::Save(const domain::Author& author) {
    // Пока каждое обращение к репозиторию выполняется внутри отдельной транзакции
    // В будущих уроках вы узнаете про паттерн Unit of Work, при помощи которого сможете несколько
    // запросов выполнить в рамках одной транзакции.
    // Вы также может самостоятельно почитать информацию про этот паттерн и применить его здесь.
    pqxx::work work{connection_};
    work.exec_params(
        R"(
        INSERT INTO authors (id, name) VALUES ($1, $2)
            ON CONFLICT (id) DO UPDATE SET name=$2;
        )"_zv,
        author.GetId().ToString(), author.GetName());
    work.commit();
}

std::vector<domain::Author> AuthorRepositoryImpl::Retrieve(void) {
    std::vector<domain::Author> authors;

    pqxx::read_transaction work{connection_};
    const auto request = work.exec_params(
        R"(SELECT id, name FROM authors 
        ORDER BY name ASC)"_zv);
    for (const auto& row : request) {
        authors.push_back(
            {domain::AuthorId::FromString(row["id"].as<std::string>()), row["name"].as<std::string>()});
    }
    return authors;
}

void BookRepositoryImpl::Save(const domain::Book& book) {
    pqxx::work work{connection_};
    work.exec_params(
        R"(
INSERT INTO books (id, title, author_id, publication_year) VALUES ($1, $2, $3, $4)
ON CONFLICT (id) DO UPDATE SET title=$2, author_id=$3, publication_year=$4;
)"_zv,
        book.GetId().ToString(),
        book.GetName(),  // В вашем book.h метод называется GetName(), хотя возвращает title
        book.GetAuthorId(), book.GetPublicationYear());
    work.commit();
}

std::vector<domain::Book> BookRepositoryImpl::RetrieveAllBooks(void) {
    std::vector<domain::Book> books;

    pqxx::read_transaction work{connection_};
    const auto request = work.exec_params(R"(
            SELECT id, title, author_id, publication_year FROM books 
            ORDER BY title ASC
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

    pqxx::read_transaction work{connection_};
    const auto request = work.exec_params(
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

Database::Database(pqxx::connection connection) : connection_{std::move(connection)} {
    pqxx::work work{connection_};
    work.exec(R"(
    CREATE TABLE IF NOT EXISTS authors (
        id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
        name varchar(100) UNIQUE NOT NULL
        );
    )"_zv);

    work.exec(R"(
    CREATE TABLE IF NOT EXISTS books (
    id UUID PRIMARY KEY,
    author_id UUID,
    title VARCHAR(100) NOT NULL,
    publication_year INT,
    CONSTRAINT fk_author
        FOREIGN KEY (author_id) 
        REFERENCES authors(id)
        );
    )"_zv);

    work.commit();
}

}  // namespace postgres