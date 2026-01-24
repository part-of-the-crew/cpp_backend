#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../src/extra_data.h"

using namespace std::literals;

SCENARIO("Extra data storing lifecycle", "[ExtraData]") {
    using extra_data::ExtraData;

    GIVEN("An empty ExtraData container") {
        ExtraData extra;

        // Check initial state immediately
        THEN("It is initially empty") {
            REQUIRE(extra.Size() == 0);
            REQUIRE_FALSE(extra.Contains("map1"));
        }
        //    void AddMapLoot(std::string name, std::vector<LootType>);
        WHEN("A valid map is added") {
            std::vector<extra_data::LootType> loot{{"key", "assets/key.obj", "obj", 90, "#338844", 0.03},
                {"wallet", "assets/wallet.obj", "obj", 0, "#883344", 0.01}};
            REQUIRE_NOTHROW(extra.AddMapLoot("map1", std::move(loot)));
            THEN("The size becomes 1 and content is retrievable") {
                REQUIRE(extra.Size() == 1);
                REQUIRE(extra.Contains("map1"));

                auto info = extra.GetMapValue("map1");
                REQUIRE(info.size() == 2);
                // Note: Ensure this matches your implementation's ID logic
                REQUIRE_NOTHROW(info.at(1).name == "wallet");
            }
            AND_THEN("check GetNumberLootforMap") {
                auto content = extra.GetNumberLootforMap("map1");
                REQUIRE(content.has_value());
                REQUIRE(content.value() == 2);
            }
            AND_THEN("check wrong map GetNumberLootforMap") {
                auto content = extra.GetNumberLootforMap("map2");
                REQUIRE_FALSE(content.has_value());
            }
        }
    }
}