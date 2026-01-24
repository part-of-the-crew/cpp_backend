#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fstream>

#include "../src/extra_data.h"
#include "../src/extra_data_serialization.h"
#include "../src/json_loader.h"

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
TEST_CASE("Parsing ExtraData logic") {
    SECTION("Throws if root is not object") {
        auto json = boost::json::parse("[]");  // Array, not object
        REQUIRE_THROWS_AS(extra_data_ser::ExtractExtraData(json), std::invalid_argument);
    }

    SECTION("Returns empty if 'maps' missing") {
        auto json = boost::json::parse("{}");
        auto result = extra_data_ser::ExtractExtraData(json);
        REQUIRE(result.Size() == 0);
    }

    SECTION("Parses valid map correctly") {
        auto json = boost::json::parse(R"({
            "maps": [
                { "id": "map1", "lootTypes": [] },
                { "id": "map2", "lootTypes": [] },
                { "id": "map3", "lootTypes": [] }
            ]
        })");

        auto result = extra_data_ser::ExtractExtraData(json);
        REQUIRE(result.Contains("map2"));
        REQUIRE(result.GetMapValue("map2") == std::vector<extra_data::LootType>{});
        REQUIRE(result.Contains("map1"));
        REQUIRE(result.GetMapValue("map1") == std::vector<extra_data::LootType>{});
        REQUIRE(result.Contains("map3"));
        REQUIRE(result.GetMapValue("map3") == std::vector<extra_data::LootType>{});
    }
}
TEST_CASE("LoadExtra integration") {
    // Setup a temp file ONCE just to prove we can read from disk
    std::filesystem::path temp_path = "temp_test_config.json";
    std::ofstream out(temp_path);
    out << R"({"maps": [{"id": "file_map"}]})";
    out.close();

    auto result = json_loader::LoadExtra(temp_path);
    REQUIRE(result.Contains("file_map"));

    std::filesystem::remove(temp_path);
}

TEST_CASE("LoadExtra real") {
    // Setup a temp file ONCE just to prove we can read from disk
    std::filesystem::path temp_path = "data/config.json";

    auto result = json_loader::LoadExtra(temp_path);
    REQUIRE(result.Contains("map1"));
    REQUIRE(result.GetNumberLootforMap("map1") == 2);
    REQUIRE(result.Contains("town"));
    REQUIRE_NOTHROW(result.GetMapValue("map1"));
}