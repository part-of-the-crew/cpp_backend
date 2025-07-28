#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <iomanip>

using namespace std;


class Db {
    std::unordered_map<int, int> user_progress_; // user -> page
    std::map<int, int> page_user_count_;         // page -> user count
public:
    void UpdateProgress(int user, int page){
        auto it = user_progress_.find(user);
        if (it != user_progress_.end()){
            if (it->second == page) return; // no change needed
            int old_page = it->second;
            if (--page_user_count_[old_page] == 0) {
                page_user_count_.erase(old_page);
            }
        }
        user_progress_[user] = page;
        ++page_user_count_[page];
    }
    double GetStat(int user) const {
        auto it = user_progress_.find(user);
        if (it == user_progress_.end())
            return 0;
        if (GetUserCount() == 1)
            return 1;
        int current_page = it->second;
        int users_behind = 0;

        for (auto iter = page_user_count_.begin(); iter != page_user_count_.lower_bound(current_page); ++iter) {
            users_behind += iter->second;
        }

        return static_cast<double>(users_behind)/static_cast<double>(GetUserCount() - 1);
    }

    size_t GetUserCount() const {
        return user_progress_.size();
    }
};


void ProcessRequests (Db &db, std::istream &in, std::ostream &out){
    int request_count;
    in >> request_count;
    for (int i = 0; i < request_count; ++i) {
        string request;
        int user;
        in >> request >> user;
        if (request == "READ"s){
            int page;
            in >> page;
            db.UpdateProgress(user, page);
            continue;
        }
        if (request == "CHEER"s){
            out << db.GetStat(user) << endl;
            continue;
        }
    }
}

int main() {
    //Let's make our processing interactive
    //This means that I print out each response as soon as I have enough information for it.
    std::cout << std::setprecision(6);
    Db db;
    ProcessRequests(db, std::cin, std::cout);

}