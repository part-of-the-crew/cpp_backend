#include <array>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <list>

using namespace std;

class VehiclePlate {
public:
    VehiclePlate(char l0, char l1, int digits, char l2, int region)
        : letters_{l0, l1, l2}
        , digits_(digits)
        , region_(region) {
    }

    string ToString() const {
        ostringstream out;
        out << letters_[0] << letters_[1];
        out << setfill('0') << right << setw(3) << digits_;
        out << letters_[2] << setw(2) << region_;

        return out.str();
    }
    //operator ==
    bool operator==(const VehiclePlate& other) const {
        return letters_ == other.letters_ && digits_ == other.digits_ && region_ == other.region_;
    }
    int Hash() const {
        return digits_;
    }

private:
    array<char, 3> letters_;
    int digits_;
    int region_;
};

ostream& operator<<(ostream& out, VehiclePlate plate) {
    out << plate.ToString();
    return out;
}

template <typename T>
class HashableContainer {
public:
    void Insert(T elem) {
        int index = elem.Hash();

        if (index >= int(elements_.size())) {
            elements_.resize(index * 2 + 1);
        }
        for ( const auto& e: elements_[index]){
            if (e == elem) {
                return;
            }
        }
        elements_[index].emplace_back(move(elem));
    }

    void PrintAll(ostream& out) const {
        for (const auto& el : elements_) {
            if (el.empty()) {
                continue;
            }
            for (const auto& e: el)
                out << e << endl;
        }
    }

    const auto& GetVector() const {
        return elements_;
    }

private:
    vector<list<T>> elements_;
};

int main() {
    HashableContainer<VehiclePlate> plate_base;
    plate_base.Insert({'B', 'H', 840, 'E', 99});
    plate_base.Insert({'O', 'K', 942, 'K', 78});
    plate_base.Insert({'O', 'K', 942, 'K', 78});
    plate_base.Insert({'O', 'K', 942, 'K', 78});
    plate_base.Insert({'O', 'K', 942, 'K', 78});
    plate_base.Insert({'H', 'E', 968, 'C', 79});
    plate_base.Insert({'T', 'A', 326, 'X', 83});
    plate_base.Insert({'H', 'H', 831, 'P', 116});
    plate_base.Insert({'P', 'M', 884, 'K', 23});
    plate_base.Insert({'O', 'C',  34, 'P', 24});
    plate_base.Insert({'M', 'Y', 831, 'M', 43});
    plate_base.Insert({'K', 'T', 478, 'P', 49});
    plate_base.Insert({'X', 'P', 850, 'A', 50});

    plate_base.PrintAll(cout);
}