#define _USE_MATH_DEFINES
#include <catch2/catch_test_macros.hpp>

#include "api_handler.h"

using namespace std::literals;
using namespace api_handler;

TEST_CASE("Correct Parse RecordSettings") {
    std::string str = "/api/v1/game/records?start=10&maxItems=50"s;

    auto result = ParseRecordSetting(str);

    REQUIRE(result);
    CHECK(result->start == 10);
    CHECK(result->maxItems == 50);
}

TEST_CASE("No Parse RecordSettings") {
    std::string str = "/api/v1/game/records"s;

    auto result = ParseRecordSetting(str);

    REQUIRE(!result);
}

TEST_CASE("THROWS Parse RecordSettings") {
    std::string str = "/api/v1/game/records?start1=10&maxItems=50"s;

    REQUIRE_THROWS_AS(ParseRecordSetting(str), std::invalid_argument);
}