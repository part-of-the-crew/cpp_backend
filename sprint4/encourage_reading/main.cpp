#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

using namespace std;


class Db {

    unordered_map <int, int> user_progress_;
    map <int, int> book_progress_; // page, amount of users
public:
    void UpdateProgress(int user, int page){
        auto it = user_progress_.find(user);
        if (it != user_progress_.end()){
            --book_progress_[it->second];
            it->second = page;
        }
        user_progress_[user] = page;
        ++book_progress_[page];
    }
    double GetStat(int user) const {
        auto user_current_page = user_progress_.find(user);
        if (user_current_page == user_progress_.end())
            return 0;
        //double percent;
        int amount_of_users = 0;
        for (auto [page, users]: book_progress_){
            if (page > user_current_page->second)
                break;
            amount_of_users += users;
        }

        return (amount_of_users)/user_progress_.size();
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
    Db db;
    ProcessRequests(db, std::cin, std::cout);

}