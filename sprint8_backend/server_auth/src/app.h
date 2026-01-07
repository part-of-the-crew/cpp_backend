#pragma once
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "model.h"
#include "tagged.h"

namespace detail {
struct TokenTag {};
}  // namespace detail

namespace app {

using namespace model;
using Token = util::Tagged<std::string, detail::TokenTag>;

class PlayerTokens {
    //...
private:
    std::random_device random_device_;
    std::mt19937_64 generator1_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
    std::mt19937_64 generator2_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
    std::string GetToken() { std::string result{std::to_string(generator1_()) + std::to_string(generator2_())}; }

    // Чтобы сгенерировать токен, получите из generator1_ и generator2_
    // два 64-разрядных числа и, переведя их в hex-строки, склейте в одну.
    // Вы можете поэкспериментировать с алгоритмом генерирования токенов,
    // чтобы сделать их подбор ещё более затруднительным
};

class Player {
public:
    Player(model::GameSession* session, model::Dog* dog) : session_{session}, dog_{dog}, token_{SetToken()} {}

    std::string GetToken() { return *token_; }

private:
    std::random_device random_device_;
    std::mt19937_64 generator1_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
    std::mt19937_64 generator2_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
    std::string SetToken() { std::string result{std::to_string(generator1_()) + std::to_string(generator2_())}; }

    model::GameSession* session_;
    model::Dog* dog_;
    Token token_;
};

struct AuthRequest {
    std::string PlayerName;
    std::string map;
};

}  // namespace app