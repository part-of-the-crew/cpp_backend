#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../src/extra_data.h"

using namespace std::literals;
namespace json = boost::json;

SCENARIO("Extra data storing") {
    using extra_data::ExtraData;

    GIVEN("Extra data") {
        ExtraData extra;

        WHEN("extra is created") {
            THEN("extra is empty") {
                INFO("checking emptiness");
                REQUIRE(extra.Size() == 0);

                AND_WHEN("add 1 member") {
                    INFO("add");
                    extra.AddMapInfo("map1", json::object{{"lootTypes", json::array{}}});
                    THEN("check size") {
                        REQUIRE(extra.Size() == 1);
                        AND_THEN("check Contains") {
                            REQUIRE(extra.Contains("map1"));
                        }
                        AND_THEN("check content") {
                            auto info = extra.GetMapValue("map1");
                            REQUIRE(info);
                            REQUIRE(*info == json::object{{"lootTypes", json::array{}}});
                        }
                        AND_WHEN("add not an object") {
                            REQUIRE_THROWS_AS(
                                extra.AddMapInfo("map1", json::parse(R"("not an object")")), std::invalid_argument);
                        }
                        AND_WHEN("clear") {
                            extra.Clear();
                            THEN("check emptiness again") {
                                INFO("should be empty");
                                REQUIRE(extra.Size() == 0);
                            }
                            AND_THEN("check Contains again ") {
                                REQUIRE(!extra.Contains("map1"));
                            }
                        }
                    }
                }
            }
        }
    }
}
