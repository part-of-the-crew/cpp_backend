#include "ParseCitySubjson.h"

using namespace std;


City CityFromJson(const JsonObject& city_obj, const Country& country) {
    return {
        city_obj.at("name"s).AsString(),
        city_obj.at("iso_code"s).AsString(),
        country.phone_code + city_obj.at("phone_code"s).AsString(),
        country.name,
        country.iso_code,
        country.time_zone,
        country.languages
    };
}

void ParseCitySubjson(vector<City>& cities, const Json& json, const Country& country) {
    for (const auto& city_json : json.AsList()) {
        cities.push_back(CityFromJson(city_json.AsObject(), country));
    }
}

// ParseCitySubjson вызывается только из функции ParseCountryJson следующим образом:
void ParseCountryJson(vector<Country>& countries, vector<City>& cities, const Json& json) {
    for (const auto& country_json : json.AsList()) {
        const auto& country_obj = country_json.AsObject();
        countries.push_back({
            country_obj["name"s].AsString(),
            country_obj["iso_code"s].AsString(),
            country_obj["phone_code"s].AsString(),
            country_obj["time_zone"s].AsString(),
        });
        Country& country = countries.back();
        for (const auto& lang_obj : country_obj["languages"s].AsList()) {
            country.languages.push_back(FromString<Language>(lang_obj.AsString()));
        }
        ParseCitySubjson(cities, country_obj["cities"s], country);
    }
}