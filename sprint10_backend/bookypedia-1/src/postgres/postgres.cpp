#include "postgres.h"
// #include <exception>
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