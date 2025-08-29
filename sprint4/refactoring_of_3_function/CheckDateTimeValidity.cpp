#include "CheckDateTimeValidity.h"
#include <chrono>
#include <stdexcept>
#include <string>

struct DateTime {
    int year, month, day, hour, minute, second;
};

void CheckDateTimeValidity(const DateTime& dt) {
    using namespace std::chrono;

    try {
        year_month_day ymd{year{dt.year}, month{dt.month}, day{dt.day}};
        if (!ymd.ok()) {
            throw std::domain_error("Invalid date");
        }

        if (dt.hour < 0 || dt.hour > 23) {
            throw std::domain_error("hour out of range");
        }
        if (dt.minute < 0 || dt.minute > 59) {
            throw std::domain_error("minute out of range");
        }
        if (dt.second < 0 || dt.second > 59) {
            throw std::domain_error("second out of range");
        }

    } catch (const std::exception& e) {
        throw;
    }
}