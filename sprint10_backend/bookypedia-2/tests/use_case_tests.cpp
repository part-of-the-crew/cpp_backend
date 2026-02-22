#include <catch2/catch_test_macros.hpp>
#include <memory>

#include "../src/app/use_cases_impl.h"
#include "../src/domain/author.h"
#include "../src/domain/book.h"
#include "../src/domain/unit_of_work.h"

namespace {

// 1. Mock Repositories (Persistence layer simulation)
struct MockAuthorRepository : domain::AuthorRepository {
    std::vector<domain::Author> saved_authors;

    void Save(const domain::Author& author) override { saved_authors.emplace_back(author); }
    std::vector<domain::Author> Retrieve() override { return saved_authors; }
    domain::Author RetrieveAuthor(const std::string& author_id) override {
        for (const auto& author : saved_authors) {
            if (author.GetId().ToString() == author_id) {
                return author;
            }
        }
        throw std::runtime_error("Author not found");
    }
    void Delete(const std::string& author_id) override {
        for (auto it = saved_authors.begin(); it != saved_authors.end(); ++it) {
            if (it->GetId().ToString() == author_id) {
                saved_authors.erase(it);
                return;
            }
        }
        throw std::runtime_error("Author not found");
    }
    void Update([[maybe_unused]] const std::string& author_id,
        [[maybe_unused]] const std::string& new_name [[maybe_unused]]) override {
        // No-op for in-memory mock
    }
};

struct MockBookRepository : domain::BookRepository {
    std::vector<domain::Book> saved_books;

    void Save(const domain::Book& book) override { saved_books.emplace_back(book); }
    std::vector<domain::Book> RetrieveAllBooks() override { return saved_books; }
    std::vector<domain::Book> RetrieveAuthorBooks(const std::string& author_id) override {
        std::vector<domain::Book> books;
        for (const auto& book : saved_books) {
            if (book.GetAuthorId() == author_id) {
                books.push_back(book);
            }
        }
        return books;
    }
    void Delete(const std::string& book_id) override {
        for (auto it = saved_books.begin(); it != saved_books.end(); ++it) {
            if (it->GetId().ToString() == book_id) {
                saved_books.erase(it);
                return;
            }
        }
        throw std::runtime_error("Book not found");
    }
    bool Update([[maybe_unused]] const std::string& book_id, [[maybe_unused]] const std::string& title,
        [[maybe_unused]] int publication_year,
        [[maybe_unused]] const std::vector<std::string>& tags) override {
        return false;
    }
    domain::Book RetrieveBook(const std::string& book_id) override {
        for (const auto& book : saved_books) {
            if (book.GetId().ToString() == book_id) {
                return book;
            }
        }
        throw std::runtime_error("Book not found");
    }
};

// 2. Mock Unit of Work
// This wrappers the repositories so UseCases can "commit" (though we just modify the vectors directly)
struct MockUnitOfWork : domain::UnitOfWork {
    MockAuthorRepository& authors_repo;
    MockBookRepository& books_repo;

    MockUnitOfWork(MockAuthorRepository& authors, MockBookRepository& books)
        : authors_repo(authors), books_repo(books) {}

    void Commit() override {
        // No-op for in-memory mock, or could set a flag like "committed = true"
    }

    domain::AuthorRepository& Authors() override { return authors_repo; }
    domain::BookRepository& Books() override { return books_repo; }
};

// 3. Test Fixture
struct Fixture {
    MockAuthorRepository authors;
    MockBookRepository books;
};

}  // namespace

SCENARIO_METHOD(Fixture, "Book Adding") {
    GIVEN("Use cases") {
        // Create the UseCases with a factory that returns a UoW pointing to our Fixture's repos
        app::UseCasesImpl use_cases{[this]() { return std::make_unique<MockUnitOfWork>(authors, books); }};

        WHEN("Adding an author") {
            const auto author_name = "Joanne Rowling";
            use_cases.AddAuthor(author_name);

            THEN("author with the specified name is saved to repository") {
                REQUIRE(authors.saved_authors.size() == 1);
                CHECK(authors.saved_authors.at(0).GetName() == author_name);
                CHECK(authors.saved_authors.at(0).GetId() != domain::AuthorId{});
            }
        }
    }
}