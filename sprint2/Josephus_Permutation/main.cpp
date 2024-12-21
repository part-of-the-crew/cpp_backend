#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>
#include <cstdint>
#include <utility>
#include <iterator>
#include <deque>

using namespace std;

template <typename RandomIt>
void MakeJosephusPermutation(RandomIt first, RandomIt last, uint32_t step_size) {
    //vector<typename RandomIt::value_type> pool(first, last);
    deque<typename RandomIt::value_type> pool1;
    for (auto it = first; it!= last; ++it) {
        pool1.emplace_back(std::move(*it));
    }
    size_t cur_pos = 0;
    while (!pool1.empty()) {
        //*(first++) = pool[cur_pos];
        //*(first++) = std::move (pool1.begin() + cur_pos);
        *(first++) = std::move(*(pool1.begin() + cur_pos));
        // Вместо копирования, мы удаляем итератор из контейнера.
        // Это позволит избежать лишних копирований при изменении итератора
        // и избежать изменения исходного контейнера, если он является
        // неизменяемым.
        // Важно отметить, что удалять итераторы из контейнера после того,
        // как вы изменяете их позицию, может привести к непредвиденным последствиям.
        // Поэтому важно учитывать эти условия при использовании итераторов.
        //
        // Вы можете использовать вместо этого кода любой другой алгоритм,
        // который позволяет изменять итераторы после того, как вы изменяете их позицию.
        // Например, вместо std::copy_backward можно использовать std::swap_ranges
        // или std::iter_swap
        //
        // Важно отметить, что удалять итераторы из контейнера после того,
        // как вы изменяете их позицию может привести к непредвиденным последствиям.
        // Поэтому важно учитывать эти условия при использовании итераторов.
        //

        //pool1.erase(pool1.begin() + cur_pos);
        if (pool1.empty()) {
            break;
        }
        cur_pos = (cur_pos + step_size - 1) % pool1.size();
    }
}

vector<int> MakeTestVector() {
    vector<int> numbers(10);
    iota(begin(numbers), end(numbers), 0);
    return numbers;
}

void TestIntVector() {
    const vector<int> numbers = MakeTestVector();
    {
        vector<int> numbers_copy = numbers;
        MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), 1);
        assert(numbers_copy == numbers);
    }
    {
        vector<int> numbers_copy = numbers;
        MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), 3);
        assert(numbers_copy == vector<int>({0, 3, 6, 9, 4, 8, 5, 2, 7, 1}));
    }
}

// Это специальный тип, который поможет вам убедиться, что ваша реализация
// функции MakeJosephusPermutation не выполняет копирование объектов.
// Сейчас вы, возможно, не понимаете как он устроен, однако мы расскажем
// об этом далее в нашем курсе

struct NoncopyableInt {
    int value;

    NoncopyableInt(const NoncopyableInt&) = delete;
    NoncopyableInt& operator=(const NoncopyableInt&) = delete;

    NoncopyableInt(NoncopyableInt&&) = default;
    NoncopyableInt& operator=(NoncopyableInt&&) = default;
};

bool operator==(const NoncopyableInt& lhs, const NoncopyableInt& rhs) {
    return lhs.value == rhs.value;
}

ostream& operator<<(ostream& os, const NoncopyableInt& v) {
    return os << v.value;
}

void TestAvoidsCopying() {
    vector<NoncopyableInt> numbers;
    numbers.push_back({1});
    numbers.push_back({2});
    numbers.push_back({3});
    numbers.push_back({4});
    numbers.push_back({5});

    MakeJosephusPermutation(begin(numbers), end(numbers), 2);

    vector<NoncopyableInt> expected;
    expected.push_back({1});
    expected.push_back({3});
    expected.push_back({5});
    expected.push_back({4});
    expected.push_back({2});

    assert(numbers == expected);
}

int main() {
    TestIntVector();
    TestAvoidsCopying();
    return 0;
}