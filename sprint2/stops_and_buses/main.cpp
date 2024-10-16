#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <set>
#include <vector>
#include <cassert>


using namespace std;

enum class QueryType {
    NewBus,
    BusesForStop,
    StopsForBus,
    AllBuses,
};

struct Query {
    QueryType type;
    string bus;
    string stop;
    vector<string> stops;
};

istream& operator>>(istream& is, Query& q) {
    // Реализуйте эту функцию
    string operation_code;
    is >> operation_code;
    if (operation_code == "NEW_BUS"s){
        q.type = QueryType::NewBus;
        is >> q.bus;
        int stop_count;
        is >> stop_count;
        q.stops.clear();
        for (int i = 0; i < stop_count; i++) {
            is >> q.stop;
            q.stops.push_back(q.stop);
        }
        return is;
    }
    if (operation_code == "BUSES_FOR_STOP"s){
        q.type = QueryType::BusesForStop;
        is >> q.stop;
        return is;
    }
    if (operation_code == "STOPS_FOR_BUS"s){
        q.type = QueryType::StopsForBus;
        is >> q.bus;
        return is;
    }
    if (operation_code == "ALL_BUSES"s){
        q.type = QueryType::AllBuses;
        return is;
    }
    throw invalid_argument(operation_code);
}

// Структура выдачи списка автобусов через остановку.
struct BusesForStopResponse {
    // Наполните полями эту структуру
    vector<string> buses_for_stop;
};

ostream& operator<<(ostream& os, const BusesForStopResponse& r) {
    // Реализуйте эту функцию
    if (r.buses_for_stop.empty()) {
        os << "No stop"s;
        return os;
    }

    bool is_first = true;
    for (const string& bus : r.buses_for_stop) {
        if (!is_first) {
            os << " "s;
        }
        is_first = false;
        os << bus;
    }
    return os;
}

// Структура выдачи описания остановок для маршрута автобуса.
struct StopsForBusResponse {
    // Наполните полями эту структуру
    //vector<string> buses_to_stops;
    map<string, vector<string>> stops_to_buses;
};

ostream& operator<<(ostream& os, const StopsForBusResponse& r) {
    // Реализуйте эту функцию
    if (r.stops_to_buses.empty()) {
        os << "No bus"s;
        return os;
    }
    bool is_first = true;
    for (const auto& [stop, buses]: r.stops_to_buses) {
        if (!is_first) {
            os << endl;
        }
        is_first = false;
        os << "Stop "s << stop << ":"s;
        //os << stop << "x"s << buses.size() << endl;
        if (buses.size() == 0) {
            os << " no interchange"s;
        } else {
            for (const string& bus : buses) {
                os << " "s << bus;
            }
        }
    }
    return os;
}

// Структура выдачи описания всех автобусов.
struct AllBusesResponse {
    // Наполните полями эту структуру
    map<string, set<string>> buses_to_stops;
};

ostream& operator<<(ostream& os, const AllBusesResponse& r) {
    // Реализуйте эту функцию
    bool is_first = true;
    if (r.buses_to_stops.empty()) {
        os << "No buses"s << endl;
    } else {
        for (const auto& bus_item : r.buses_to_stops) {
            if (!is_first) {
                os << endl;
            }
            is_first = false;
            os << "Bus "s << bus_item.first << ":"s;
            for (const string& stop : bus_item.second) {
                os << " "s << stop;
            }
        }
    }    
    return os;
}

class BusManager {
public:
    void AddBus(const string& bus, const vector<string>& stops) {
        // Реализуйте этот метод
        //vector<string>& stopsR = buses_to_stops[bus];
        for (const string& stop : stops) {
            buses_to_stops[bus].insert(stop);
            stops_to_buses[stop].insert(bus);
            //cout << "x" << bus << buses_to_stops.size() << stop << stops_to_buses.size() << endl;
        }   
    }

    BusesForStopResponse GetBusesForStop(const string& stop) const {
        // Реализуйте этот метод
        BusesForStopResponse response;
        auto it = stops_to_buses.find(stop);
        if (it == stops_to_buses.end()) {
            return response;
        }

        //copy from it.second to response.buses_for_stop
        /*
        response.buses_for_stop.reserve(it->second.size());
        for (const string& bus : it->second) {
            response.buses_for_stop.push_back(bus);
        }
*/
        copy(it->second.begin(), it->second.end(), back_inserter(response.buses_for_stop));

        return response;
    }

    StopsForBusResponse GetStopsForBus(const string& bus) const {
        // Реализуйте этот метод
        StopsForBusResponse response;
        auto it = buses_to_stops.find(bus);
        if (it == buses_to_stops.end()) {
            return response;
        }

        for (const string& stop : it->second) {
            auto it1 = stops_to_buses.find(stop);
            if (it1 != stops_to_buses.end()) {
                for (const string& other_bus : it1->second) {
                    //cout << "_" << stop << other_bus << bus << endl;
                    auto &other = response.stops_to_buses[stop];
                    if (bus != other_bus) {
                        other.push_back(other_bus);
                    }
                }
            }
        }
        return response;
    }

    AllBusesResponse GetAllBuses() const {
        // Реализуйте этот метод
        AllBusesResponse response{buses_to_stops};
        //copy(stops_to_buses.begin(), stops_to_buses.end(), back_inserter(response.buses_to_stops));
        return response;
    }
private:
    map<string, set<string>> stops_to_buses;
    map<string, set<string>> buses_to_stops;
};

// Реализуйте функции и классы, объявленные выше, чтобы эта функция main
// решала задачу "Автобусные остановки"

int main() {
    int query_count;
    Query q;

    cin >> query_count;

    BusManager bm;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
            case QueryType::NewBus:
                bm.AddBus(q.bus, q.stops);
                break;
            case QueryType::BusesForStop:
                cout << bm.GetBusesForStop(q.stop) << endl;
                break;
            case QueryType::StopsForBus:
                cout << bm.GetStopsForBus(q.bus) << endl;
                break;
            case QueryType::AllBuses:
                cout << bm.GetAllBuses() << endl;
                break;
        }
    }
}





/*
int main() {
    int q;
    cin >> q;

    map<string, vector<string>> buses_to_stops, stops_to_buses;

    for (int i = 0; i < q; ++i) {
        string operation_code;
        cin >> operation_code;

        if (operation_code == "NEW_BUS"s) {
            string bus;
            cin >> bus;
            int stop_count;
            cin >> stop_count;
            vector<string>& stops = buses_to_stops[bus];
            stops.resize(stop_count);
            for (string& stop : stops) {
                cin >> stop;
                stops_to_buses[stop].push_back(bus);
            }

        } else if (operation_code == "BUSES_FOR_STOP"s) {
            string stop;
            cin >> stop;
            if (stops_to_buses.count(stop) == 0) {
                cout << "No stop"s << endl;
            } else {
                bool is_first = true;
                for (const string& bus : stops_to_buses[stop]) {
                    if (!is_first) {
                        cout << " "s;
                    }
                    is_first = false;
                    cout << bus;
                }
                cout << endl;
            }

        } else if (operation_code == "STOPS_FOR_BUS"s) {
            string bus;
            cin >> bus;
            if (buses_to_stops.count(bus) == 0) {
                cout << "No bus"s << endl;
            } else {
                for (const string& stop : buses_to_stops[bus]) {
                    cout << "Stop "s << stop << ":"s;
                    if (stops_to_buses[stop].size() == 1) {
                        cout << " no interchange"s;
                    } else {
                        for (const string& other_bus : stops_to_buses[stop]) {
                            if (bus != other_bus) {
                                cout << " "s << other_bus;
                            }
                        }
                    }
                    cout << endl;
                }
            }

        } else if (operation_code == "ALL_BUSES"s) {
            if (buses_to_stops.empty()) {
                cout << "No buses"s << endl;
            } else {
                for (const auto& bus_item : buses_to_stops) {
                    cout << "Bus "s << bus_item.first << ":"s;
                    for (const string& stop : bus_item.second) {
                        cout << " "s << stop;
                    }
                    cout << endl;
                }
            }
        }
    }
}
*/