#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

class RouteManager {
public:

    int get_nearest_point_to_target(const set<int>& s, int target) const {
        auto it = s.lower_bound(target);
        int result = target;
        if (it == s.begin()) {
            // Only 'it' can be closest
            result = *it;
        } 
        else if (it == s.end()) {
            // Only previous element can be closest
            auto prev_it = prev(it);
            result = *prev_it;
        } 
        else {
            // Compare current and previous
            auto prev_it = prev(it);
            if (abs(*it - target) < abs(*prev_it- target))
                result = *it;
            else
                result = *prev_it;
        }
        return result;
    }

    void AddRoute(int start, int finish) {
        reachable_lists_[start].insert(finish);
        reachable_lists_[finish].insert(start);
    }
    int FindNearestFinish(int start, int finish) const {
        int result = abs(start - finish);
        if (!reachable_lists_.contains(start)) {
            return result;
        }
        const set<int>& s = reachable_lists_.at(start);
        if (s.empty())
            return result;

        return std::min (std::abs(get_nearest_point_to_target(s, finish) - finish), result);
    }

private:
    map<int, set<int>> reachable_lists_;
};


inline void Tests() {
    {
        RouteManager routes;
        set<int> s = {1, 2, 3, 3, 5, 6, 10};
        // Simulated program-created variable
        //auto result = routes.get_nearest_point_to_target(s, 1);

        assert(routes.get_nearest_point_to_target(s, 1) == 1);
        assert(routes.get_nearest_point_to_target(s, 0) == 1);
        assert(routes.get_nearest_point_to_target(s, -2) == 1);
        assert(routes.get_nearest_point_to_target(s, 4) == 3);
        assert(routes.get_nearest_point_to_target(s, 11) == 10);
        assert(routes.get_nearest_point_to_target(s, 9) == 10);
        assert(routes.get_nearest_point_to_target(s, 8) == 6);
    }

}
int main() {

    Tests();

    RouteManager routes;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;
        int start, finish;
        cin >> start >> finish;
        if (query_type == "ADD"s) {
            routes.AddRoute(start, finish);
        } else if (query_type == "GO"s) {
            cout << routes.FindNearestFinish(start, finish) << "\n"s;
        }
    }
}