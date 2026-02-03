#include "serialization.h"

namespace serialization {

ApplicationRepr::ApplicationRepr(const app::Application& app) {
    // 1. Save all Dogs (grouped by Map)
    // We iterate over player tokens to find all active dogs.
    // Alternatively, if Application has a way to iterate all sessions, use that.
    // Assuming we can access player_tokens_ via friendship or getter:

    for (const auto& [token, player] : app.player_tokens_) {
        // Save the Player Token mapping
        std::string map_id = *player.GetSession()->GetMap()->GetId();
        auto dog_id = player.GetDog()->GetId();
        player_reprs_[token] = {map_id, dog_id};

        // Save the Dog itself (if not already saved)
        // Note: This logic assumes 1 dog per player.
        // If there are NPC dogs, you'd iterate sessions instead.
        dog_reprs_[map_id].push_back(DogRepr(*player.GetDog()));
    }

    // 2. Save Loot
    for (const auto& [map_id, loots] : app.loots_) {
        for (const auto& loot : loots) {
            loot_reprs_[map_id].emplace_back(loot);
        }
    }
}

void ApplicationRepr::Restore(app::Application& app) const {
    // 1. Restore Game Sessions and Dogs
    for (const auto& [map_id_str, dogs] : dog_reprs_) {
        model::Map::Id map_id{map_id_str};

        // Ensure session exists (this creates it if missing)
        model::GameSession* session = app.game_.FindSession(map_id);
        if (!session)
            continue;  // Should not happen if config matches

        for (const auto& dog_repr : dogs) {
            // We need a way to inject a fully formed Dog into the session.
            // Standard AddDog() creates a new one. You might need a specific RestoreDog method
            // in GameSession or modify AddDog to take a Dog object.

            model::Dog restored_dog = dog_repr.Restore();

            // HACK: Since GameSession::AddDog returns a pointer to a new dog,
            // we might need to modify GameSession to allow inserting an existing dog
            // OR add a dummy dog and then overwrite it.

            // Recommended: Add `void AddDog(Dog dog)` to GameSession
            session->AddDog(std::move(restored_dog));
        }
    }

    // 2. Restore Player Tokens (Reconnect tokens to the dogs we just created)
    for (const auto& [token, pair] : player_reprs_) {
        std::string map_id_str = pair.first;
        int dog_id = pair.second;

        model::Map::Id map_id{map_id_str};
        model::GameSession* session = app.game_.FindSession(map_id);

        // Find the specific dog by ID in the session
        model::Dog* found_dog = nullptr;
        for (auto& dog : session->GetDogs()) {
            if (dog.GetId() == dog_id) {
                found_dog = &dog;
                break;
            }
        }

        if (found_dog) {
            // Re-register the player
            app::Player player{session, found_dog};
            app.player_tokens_.AddTokenUnsafe(token, player);  // You need a method to force a specific token
        }
    }

    // 3. Restore Loot
    for (const auto& [map_id, loots] : loot_reprs_) {
        for (const auto& loot_repr : loots) {
            app.loots_[map_id].push_back(loot_repr.Restore());
        }
    }
}

}  // namespace serialization