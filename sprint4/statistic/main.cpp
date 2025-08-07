#include <algorithm>
#include <cassert>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

using namespace std;

// напишите функцию ComputeStatistics, принимающую 5 параметров:
// два итератора, выходное значение для суммы, суммы квадратов и максимального элемента

struct OnlySum {
    int value;
};

OnlySum operator+(OnlySum l, OnlySum r) {
    return {l.value + r.value};
}
OnlySum& operator+=(OnlySum& l, OnlySum r) {
    return l = l + r;
}
/*
    сумму,
    сумму квадратов,
    максимальный элемент.
*/

template <typename InputIt, typename OutSum, typename OutSqSum, typename OutMax>
void ComputeStatistics(InputIt first, InputIt last, OutSum& out_sum, OutSqSum& out_sq_sum, OutMax& out_max){
    using Elem = std::decay_t<decltype(*first)>;
    constexpr bool need_sum_opt = is_same_v<OutSum, Elem>;
    constexpr bool need_sum = !is_same_v<OutSum, const nullopt_t>;

    constexpr bool need_sq_opt = is_same_v<OutSqSum, Elem>;
    constexpr bool need_sq_sum = !is_same_v<OutSqSum, const nullopt_t>;

    constexpr bool need_max_opt = is_same_v<OutMax, Elem>;
    constexpr bool need_max = !is_same_v<OutMax, const nullopt_t>;

    if constexpr (need_max)
        out_max = *first;

    if constexpr (need_sq_sum)
        out_sq_sum = *first * *first;

    if constexpr (need_sum){
        if constexpr (need_sum_opt){
            out_sum = *first;
        } else {
            *out_sum = *first;
        }
    }

    for (auto it = first; it != last; it++){

        if (it == first)
            continue;

        if constexpr (need_max){
            if constexpr (need_max_opt){
                if (*it > out_max)
                    out_max = *it;
            } else {
                if (*it > *out_max)
                    *out_max = *it;
            }
        }
        
        if constexpr (need_sum){
            if constexpr (need_sum_opt){
                    out_sum += *it;
            } else {
                *out_sum += *it;
            }
        }

        if constexpr (need_sq_sum){
            if constexpr (need_sq_opt){
                out_sq_sum += *it * *it;
            } else {
                *out_sq_sum += *it * *it;
            }
        }
    }
}

int main() {
    vector input = {1, 2, 3, 4, 5, 6};
    int sq_sum;
    std::optional<int> max;

    // Переданы выходные параметры разных типов - std::nullopt_t, int и std::optional<int>
    ComputeStatistics(input.begin(), input.end(), nullopt, sq_sum, nullopt);

    //assert(max && *max == 6);
    assert(sq_sum == 91);

    vector<OnlySum> only_sum_vector = {{100}, {-100}, {20}};
    OnlySum sum;

    // Поданы значения поддерживающие только суммирование, но запрошена только сумма
    ComputeStatistics(only_sum_vector.begin(), only_sum_vector.end(), sum, nullopt, nullopt);

    assert(sum.value == 20);
}