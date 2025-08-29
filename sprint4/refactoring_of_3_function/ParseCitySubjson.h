#pragma once

// Дана функция ParseCitySubjson, обрабатывающая JSON-объект со списком городов конкретной страны:
void ParseCitySubjson(vector<City>& cities, const Json& json, const string& country_name,
                      const string& country_iso_code, const string& country_phone_code, const string& country_time_zone,
                      const vector<Language>& languages);

// ParseCitySubjson вызывается только из функции ParseCountryJson следующим образом:
void ParseCountryJson(vector<Country>& countries, vector<City>& cities, const Json& json);