#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>

#include "log_duration.h"

using namespace std;

class RandomContainer {
public:
    void Insert(int val) {
        try {
            values_map_[val] = values_pool_.size() - 1;
        } catch (...) {
            // Rollback insertion into values_pool_
            values_pool_.pop_back();
            throw;
        }
    }
    void Remove(int val) {
        auto it1 = values_map_.find(val);
        auto it2 = values_map_.find(values_pool_.back());

        std::swap(values_pool_[it1->second], values_pool_[it2->second]);

        it2->second = it1->second;
        values_map_.erase(it1);
        values_pool_.pop_back();
    }
    bool Has(int val) const {
        if (values_map_.contains(val))
            return true;
        return false;
    }
    int GetRand() const {
        uniform_int_distribution<int> distr(0, values_pool_.size() - 1);
        int rand_index = distr(engine_);
        return values_pool_[rand_index];
    }

private:
    vector<int> values_pool_;
    unordered_map<int, int> values_map_; // num, pos
    mutable mt19937 engine_;
};

int main() {
    RandomContainer container;
    int query_num = 0;
    cin >> query_num;
    {
        LOG_DURATION("Requests handling"s);
        for (int query_id = 0; query_id < query_num; query_id++) {
            string query_type;
            cin >> query_type;
            if (query_type == "INSERT"s) {
                int value = 0;
                cin >> value;
                container.Insert(value);
            } else if (query_type == "REMOVE"s) {
                int value = 0;
                cin >> value;
                container.Remove(value);
            } else if (query_type == "HAS"s) {
                int value = 0;
                cin >> value;
                if (container.Has(value)) {
                    cout << "true"s << endl;
                } else {
                    cout << "false"s << endl;
                }
            } else if (query_type == "RAND"s) {
                cout << container.GetRand() << endl;
            }
        }
    }
}