#pragma once


vector<Person> LoadPersons(string_view db_name, int db_connection_timeout, bool db_allow_exceptions,
                           DBLogLevel db_log_level, int min_age, int max_age, string_view name_filter);