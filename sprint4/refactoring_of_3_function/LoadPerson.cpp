#include "LoadPerson.h"
#include <string_view>
#include <string>
#include <vector>

using namespace std;
using namespace std::string_literals;

struct DBConfig {
    string_view name;
    int connection_timeout;
    bool allow_exceptions;
    DBLogLevel log_level;
};

struct PersonQueryParams {
    int min_age;
    int max_age;
    string_view name_filter;
};

DBHandler ConnectToDatabase(string_view db_name,
                                   int db_connection_timeout,
                                   bool db_allow_exceptions,
                                   DBLogLevel db_log_level)
{
    DBConnector connector(db_allow_exceptions, db_log_level);

    if (db_name.starts_with("tmp."s)) {
        return connector.ConnectTmp(db_name, db_connection_timeout);
    } else {
        return connector.Connect(db_name, db_connection_timeout);
    }
}

// Separate helper to build the query string
DBQuery BuildPersonQuery(DBHandler& db,
                                int min_age,
                                int max_age,
                                string_view name_filter)
{
    ostringstream query_str;
    query_str << "from Persons "
              << "select Name, Age "
              << "where Age between " << min_age << " and " << max_age << " "
              << "and Name like '%" << db.Quote(name_filter) << "%'";
    return DBQuery(query_str.str());
}

vector<Person> LoadPersons(const DBConfig& db_config,
                           const PersonQueryParams& query_params)
{
    DBHandler db = ConnectToDatabase(db_config.name,
                                     db_config.connection_timeout,
                                     db_config.allow_exceptions,
                                     db_config.log_level);

    if (!db_config.allow_exceptions && !db.IsOK()) {
        return {};
    }

    DBQuery query = BuildPersonQuery(db,
                                     query_params.min_age,
                                     query_params.max_age,
                                     query_params.name_filter);

    vector<Person> persons;
    for (auto [name, age] : db.LoadRows<string, int>(query)) {
        persons.emplace_back(move(name), age);
    }

    return persons;
}
