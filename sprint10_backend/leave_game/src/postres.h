#pragma once
#include <pqxx/pqxx>

#include "connection_pool.h"
#include "retired_player.h"

namespace postgres {

class RetiredPlayerRepositoryImpl : public domain::RetiredPlayerRepository {
public:
    explicit RetiredPlayerRepositoryImpl(pqxx::work& transaction) : transaction_(transaction) {}

    void Save(const domain::RetiredPlayer& player) override {
        transaction_.exec_params("INSERT INTO retired_players (name, score, play_time) VALUES ($1, $2, $3);",
            player.name, player.score, player.play_time);
    }

    std::vector<domain::RetiredPlayer> GetTop(int start, int max_items) override {
        const auto rows = transaction_.exec_params(
            "SELECT name, score, play_time FROM retired_players "
            "ORDER BY score DESC, play_time ASC, name ASC "
            "LIMIT $1 OFFSET $2;",
            max_items, start);

        std::vector<domain::RetiredPlayer> result;
        for (const auto& row : rows) {
            result.push_back({row[0].as<std::string>(), row[1].as<int>(), row[2].as<double>()});
        }
        return result;
    }

private:
    pqxx::work& transaction_;
};

class UnitOfWorkImpl : public domain::UnitOfWork {
public:
    // Change parameter type to ConnectionWrapper
    explicit UnitOfWorkImpl(connection_pool::ConnectionPool::ConnectionWrapper conn)
        : conn_(std::move(conn)), transaction_(*conn_) {}

    void Commit() override { transaction_.commit(); }

    domain::RetiredPlayerRepository& RetiredPlayers() override { return repo_; }

private:
    // Store the wrapper, NOT the raw ConnectionPtr
    connection_pool::ConnectionPool::ConnectionWrapper conn_;
    pqxx::work transaction_;
    RetiredPlayerRepositoryImpl repo_{transaction_};
};

inline void SetupDatabase(pqxx::connection& conn) {
    pqxx::work tx{conn};
    tx.exec(R"(
        CREATE TABLE IF NOT EXISTS retired_players (
            id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
            name VARCHAR(100) NOT NULL,
            score INTEGER NOT NULL,
            play_time DOUBLE PRECISION NOT NULL
        );
        CREATE INDEX IF NOT EXISTS idx_retired_players_sort 
        ON retired_players (score DESC, play_time ASC, name ASC);
    )");
    tx.commit();
}

}  // namespace postgres