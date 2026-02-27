#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace domain {

struct RetiredPlayer {
    std::string name;
    int score;
    double play_time;
};

class RetiredPlayerRepository {
public:
    virtual void Save(const RetiredPlayer& player) = 0;
    virtual std::vector<RetiredPlayer> GetTop(int start, int max_items) = 0;
    virtual ~RetiredPlayerRepository() = default;
};

class UnitOfWork {
public:
    virtual void Commit() = 0;
    virtual RetiredPlayerRepository& RetiredPlayers() = 0;
    virtual ~UnitOfWork() = default;
};

using UnitOfWorkFactory = std::function<std::unique_ptr<UnitOfWork>()>;

}  // namespace domain