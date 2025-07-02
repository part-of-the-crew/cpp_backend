#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <ranges>
#include <unordered_map>

using namespace std;

template<typename T>
concept BasicString = std::is_same_v<T, std::basic_string<typename T::value_type>>;

template<typename T>
concept StringLike = BasicString<T> || std::is_convertible_v<T, std::string>;

template<typename T>
concept range = std::is_class_v<T> && !StringLike<T>;

template <range C>
void print(const C& s) {
    for (const auto& e : s)
        std::cout << e << " ";
    if (!s.empty())
        std::cout << std::endl;
}


template <typename C> 
void print(const C& s) {
    std::cout << s << "";
    std::cout << std::endl;
}

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end)
        : first(begin)
        , last(end) {
    }

    Iterator begin() const {
        return first;
    }

    Iterator end() const {
        return last;
    }

private:
    Iterator first, last;
};

template <typename Collection>
auto Head(Collection& v, size_t top) {
    return IteratorRange{v.begin(), next(v.begin(), min(top, v.size()))};
}

struct Person {
    string name;
    int age;
    int income;
    bool is_male;
};
std::ostream& operator<<(std::ostream& out, const Person& person) {
    out << "Name: " << person.name
        << ", Age: " << person.age
        << ", Income: " << person.income
        << ", Is male: " << std::boolalpha << person.is_male
        << std::endl;
    return out;
}

vector<Person> ReadPeople(istream& input) {
    int count;
    input >> count;
    input.ignore();
//Ivan 25 1000 M
    vector<Person> result(count);
    for (Person& p : result) {
        char gender;
        input >> p.name >> p.age >> p.income >> gender;
        p.is_male = gender == 'M';
    }
    input.ignore();
    return result;
}

int main() {
    std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);
    vector<Person> people = ReadPeople(cin);

    sort(people.begin(), people.end(), 
    	[](const Person& lhs, const Person& rhs) {
        return lhs.age < rhs.age;
    });
    auto people_in{people};
    sort(people_in.begin(), people_in.end(),
            	[](const Person& lhs, const Person& rhs) {
                return lhs.income > rhs.income;
            });

    for (string command; cin >> command;) {
        if (command == "AGE"s) {
            int adult_age;
            cin >> adult_age;
            auto adult_begin = lower_bound(people.begin(), people.end(), adult_age, 
            	[](const Person& lhs, int age) {
                return lhs.age < age;
            });

            cout << "There are "s << distance(adult_begin, people.end()) << " adult people for maturity age "s
                 << adult_age << '\n';
        } else if (command == "WEALTHY"s) {
            int count;
            cin >> count;
            auto head = Head(people_in, count);

            int total_income = accumulate(head.begin(), head.end(), 0, [](int cur, Person& p) {
                return cur + p.income;;
            });
            cout << "Top-"s << count << " people have total income "s << total_income << '\n';
        } else if (command == "POPULAR_NAME"s) {
            char gender;
            cin >> gender;
            std::unordered_map<string, int> name_freq;
            int max_count = 0;
            string most_popular;

            for (const Person& p : people) {
                if (p.is_male == (gender == 'M')) {
                    int count = ++name_freq[p.name];
                    if (count > max_count || (count == max_count && p.name < most_popular)) {
                        max_count = count;
                        most_popular = p.name;
                    }
                }
            }

            if (max_count == 0) {
                cout << "No people of gender "s << gender << '\n';
            } else {
                cout << "Most popular name among people of gender "s << gender << " is "s << most_popular << '\n';
            }
        }
    }
        
}