#include "app.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace app {

using namespace std::literals;

Token PlayerTokens::GenerateToken() {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << generator1_() << generator2_();
    return ss.str();
}

Token PlayerTokens::AddPlayer(Player player) {
    Token token;
    do {
        token = GenerateToken();
    } while (token_to_player_.contains(token));

    token_to_player_.emplace(token, std::move(player));
    return token;
}

Player* PlayerTokens::FindPlayer(Token token) {
    if (auto it = token_to_player_.find(token); it != token_to_player_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::optional<JoinGameResult> Application::JoinGame(const AuthRequest& authReq) {
    // 1. Find the map
    const auto* map = game_.FindMap(model::Map::Id{authReq.map});
    if (!map) {
        return std::nullopt;
    }

    // 2. Get or create a session for the map
    auto* session = game_.FindSession(model::Map::Id{authReq.map});
    if (!session) {
        return std::nullopt;
    }

    // 3. Create a dog in the session
    auto* dog = session->AddDog(authReq.playerName);

    // 4. Create a player and bind to token
    Player player{session, dog};
    Token token = player_tokens_.AddPlayer(player);

    return JoinGameResult{token, dog->GetId()};
}

std::vector<Player> Application::GetPlayers(const Token& token) {
    Player* player = player_tokens_.FindPlayer(token);
    if (!player) {
        throw std::invalid_argument("Invalid token");
    }

    // Get all dogs in the session of the requesting player
    const auto& dogs = player->GetSession()->GetDogs();

    std::vector<Player> result;
    // We construct players on the fly for the view,
    // but typically you might just return structs with name/id.
    // For now, let's just use the Player class or simpler structs.
    // Actually, looking at the request, we just need to list them.
    // Let's return the dogs directly or wrap them.

    // Important: The list of players logic usually requires returning
    // names and IDs. The Player class has accessors for that.

    // Re-constructing Player objects might be weird if they don't have tokens,
    // but strictly speaking, "GetPlayers" returns data about OTHER players.
    // Let's change the return type to be simpler in a real app,
    // but here we will return the subset of data needed.

    for (const auto& dog : dogs) {
        // We create temporary Player views for the dogs in the session
        // Note: These temporary players don't have their own tokens tracked here
        // This is sufficient for reading data (GetName, GetId)
        result.emplace_back(const_cast<model::GameSession*>(player->GetSession()), const_cast<model::Dog*>(&dog));
    }
    return result;
}

}  // namespace app