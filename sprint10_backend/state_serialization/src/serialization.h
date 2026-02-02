#pragma once
#include <cstddef>
// #include <iostream>
#include <boost/serialization/vector.hpp>
#include <string>
#include <vector>

#include "app.h"
#include "geom.h"
#include "model.h"

namespace geom {

template <typename Archive>
void serialize(Archive& ar, Point2D& point, [[maybe_unused]] const unsigned version) {
    ar & point.x;
    ar & point.y;
}

template <typename Archive>
void serialize(Archive& ar, Size& s, [[maybe_unused]] const unsigned version) {
    ar & s.width;
    ar & s.height;
}

template <typename Archive>
void serialize(Archive& ar, Rectangle& rec, [[maybe_unused]] const unsigned version) {
    ar & rec.position;
    ar & rec.size;
}

template <typename Archive>
void serialize(Archive& ar, Offset& s, [[maybe_unused]] const unsigned version) {
    ar & s.dx;
    ar & s.dy;
}

template <typename Archive>
void serialize(Archive& ar, Position& p, [[maybe_unused]] const unsigned version) {
    ar & p.x;
    ar & p.y;
}

template <typename Archive>
void serialize(Archive& ar, Speed& s, [[maybe_unused]] const unsigned version) {
    ar & s.ux;
    ar & s.uy;
}

template <typename Archive>
void serialize(Archive& ar, Direction& d, [[maybe_unused]] const unsigned version) {
    ar & d;
}

}  // namespace geom

namespace model {

template <typename Archive>
void serialize(Archive& ar, BagItem& obj, [[maybe_unused]] const unsigned version) {
    ar&(obj.id);
    ar&(obj.type);
}

}  // namespace model

namespace serialization {

// DogRepr (DogRepresentation) - сериализованное представление класса Dog
class DogRepr {
public:
    DogRepr() = default;

    explicit DogRepr(const model::Dog& dog)
        : name_(dog.GetName())
        , id_(dog.GetId())
        , pos_(dog.GetPosition())
        , bag_capacity_(dog.GetBagCapacity())
        , speed_(dog.GetSpeed())
        , direction_(dog.GetDirection())
        , score_(dog.GetScore())
        , bag_content_(dog.GetBagContent()) {}

    [[nodiscard]] model::Dog Restore() const {
        model::Dog dog{name_, id_, pos_, bag_capacity_};
        dog.SetSpeed(speed_);
        dog.SetDirection(direction_);
        dog.AddScore(score_);
        for (const auto& item : bag_content_) {
            if (!dog.AddToBag(item)) {
                throw std::runtime_error("Failed to put bag content");
            }
        }
        return dog;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar & name_;
        ar & id_;
        ar & pos_;
        ar & bag_capacity_;
        ar & speed_;
        ar & direction_;
        ar & score_;
        ar & bag_content_;
    }

private:
    std::string name_;
    size_t id_ = 0;
    geom::Position pos_;
    size_t bag_capacity_ = 0;
    geom::Speed speed_;
    geom::Direction direction_ = geom::Direction::NORTH;
    size_t score_ = 0;
    model::Dog::BagContent bag_content_;
};

class GameSessionRepr {
public:
    GameSessionRepr() = default;

    explicit GameSessionRepr(const model::GameSession& session)
        : dogs_(session.GetDogs().size()), map_(session.GetMap()) {
        for (size_t i = 0; i < session.GetDogs().size(); ++i) {
            dogs_[i] = DogRepr(session.GetDogs()[i]);
        }
    }

    [[nodiscard]] model::GameSession Restore() const {
        model::GameSession session{map_};
        for (const auto& dog : dogs_) {
            session.AddDog(dog.Restore().GetName());
        }
        return session;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar & dogs_;
        ar & map_;
    }

private:
    std::vector<DogRepr> dogs_;
    const model::Map* map_{nullptr};
};

class GameRepr {
public:
    GameRepr() = default;

    explicit GameRepr(const model::Game& game) : maps_(game.GetMaps()) {}

    void Restore(model::Game& game) const {
        model::Game game;
        for (const auto& dog : dogs_) {
            game.AddDog(dog.Restore().GetName());
        }
        return game;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar & dogs_;
    }

private:
    std::vector<DogRepr> dogs_;
};

}  // namespace serialization
