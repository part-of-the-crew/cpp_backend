#include <iostream>
#include <deque>
#include <unordered_set>
#include <algorithm>
#include <optional>
#include <string>
#include <numeric>

using namespace std;

struct Booking {
    int64_t time;
    string hotel_name;
    int client_id;
    int room_count;
/*
    bool operator<(const Booking& rhs) {
        return client_id < rhs.client_id;
    };
    bool operator<(const Booking& rhs) const {
        return client_id < rhs.client_id;
    };
*/
    bool operator==(const Booking& other) const {
        return (other.client_id == client_id);
    };

};
struct IntHasher {
    size_t operator() (const Booking &b) const {
        return std::hash<int>()(b.client_id);
    }
};

class HotelManager {
public:
    void Book(Booking booking) {
        for (auto it = bookings_.begin(); it < bookings_.end(); it++){
            if (it->time <= (booking.time - 86400)){
                bookings_.erase(it);
            } else {
                break;
            }
        }
        bookings_.push_back(booking);
    };
    int ComputeClientCount(string hotel_name) {
        unordered_set<Booking, IntHasher> bookings;
        for (auto const& booking: bookings_){
            if (booking.hotel_name == hotel_name){
                bookings.insert(booking);
            }
        }
        return bookings.size();
    };
    int ComputeRoomCount(string hotel_name){
        return accumulate(bookings_.begin(), bookings_.end(), 0, [&hotel_name](int cur, const Booking& b) {
            if (b.hotel_name == hotel_name)
                return cur + b.room_count;
            return cur;
        });
    };

private:
    deque <Booking> bookings_;
};

int main() {
    HotelManager manager;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;

        if (query_type == "BOOK") {
            int64_t time;
            cin >> time;
            string hotel_name;
            cin >> hotel_name;
            int client_id, room_count;
            cin >> client_id >> room_count;
            manager.Book({time, hotel_name, client_id, room_count});
        } else {
            string hotel_name;
            cin >> hotel_name;
            if (query_type == "CLIENTS") {
                cout << manager.ComputeClientCount(hotel_name) << "\n";
            } else if (query_type == "ROOMS") {
                cout << manager.ComputeRoomCount(hotel_name) << "\n";
            }
        }
    }

    return 0;
}