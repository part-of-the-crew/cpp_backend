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
            --page_user_count_[it->second];
            it->second = page;
        }
        user_progress_[user] = page;
        ++page_user_count_[page];
    }
    double GetStat(int user) const {
        auto user_current_page = user_progress_.find(user);
        if (user_current_page == user_progress_.end())
            return 0;
        if (user_progress_.size() == 1)
            return 1;
        int amount_of_users = 0;
        for (auto [page, users]: page_user_count_){
            if (page >= user_current_page->second)
                break;
            amount_of_users += users;
        }

        return static_cast<double>(amount_of_users)/static_cast<double>(user_progress_.size() - 1);
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