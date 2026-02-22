#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include "../domain/unit_of_work.h"

namespace postgres {

class AuthorRepositoryImpl : public domain::AuthorRepository {
public:
    // Now accepts a transaction (work), not a raw connection
    explicit AuthorRepositoryImpl(pqxx::work& work) : work_{work} {}

    void Save(const domain::Author& author) override;
    std::vector<domain::Author> Retrieve() override;
    domain::Author RetrieveAuthor(const std::string& author_id) override;
    void Delete(const std::string& author_id) override;
    void Update(const std::string& author_id, const std::string& new_name) override;

private:
    pqxx::work& work_;
};

class BookRepositoryImpl : public domain::BookRepository {
public:
    explicit BookRepositoryImpl(pqxx::work& work) : work_{work} {}

    void Save(const domain::Book& book) override;
    std::vector<domain::Book> RetrieveAllBooks() override;
    std::vector<domain::Book> RetrieveAuthorBooks(const std::string& author_id) override;
    void Delete(const std::string& book_id) override;
    bool Update(const std::string& book_id, const std::string& title, int publication_year,
        const std::vector<std::string>& tags) override;
    domain::Book RetrieveBook(const std::string& book_id) override;

private:
    pqxx::work& work_;
};

class UnitOfWorkImpl : public domain::UnitOfWork {
public:
    explicit UnitOfWorkImpl(pqxx::connection& connection)
        : work_(connection), authors_(work_), books_(work_) {}

    void Commit() override { work_.commit(); }

    domain::AuthorRepository& Authors() override { return authors_; }
    domain::BookRepository& Books() override { return books_; }

private:
    pqxx::work work_;
    AuthorRepositoryImpl authors_;
    BookRepositoryImpl books_;
};

class Database {
public:
    explicit Database(pqxx::connection connection);

    pqxx::connection& GetConnection() { return connection_; }

private:
    pqxx::connection connection_;
};

}  // namespace postgres