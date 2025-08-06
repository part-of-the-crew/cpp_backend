#pragma once

#include "date.h"
#include <numeric>  // for std::iota
#include <iostream>

class BudgetManager {
    std::vector <double> earn_days;
    std::vector <double> spend_days;
public:
    static const Date START_DATE;
    static const Date END_DATE;

    BudgetManager ()
        : earn_days(Date::ComputeDistance(START_DATE, END_DATE))
        , spend_days(earn_days)
        {}  

    void Earn(const Date& begin_date, const Date& end_date, int earning){
        auto dist_start_begin = Date::ComputeDistance(START_DATE, begin_date);
        auto dist_begin_end = Date::ComputeDistance(begin_date, end_date) + 1;
        double daily = static_cast<double>(earning) / dist_begin_end;

        //std::cout << dist_start_begin << " " << dist_begin_end << std::endl;
        for (auto i = dist_start_begin; i < dist_begin_end + dist_start_begin; ++i) {
            earn_days[i] += daily;
                    //std::cout << "     " << daily << std::endl;
        }
        return;
    }
    void Spend (const Date& begin_date, const Date& end_date, int spending){
        auto dist_start_begin = Date::ComputeDistance(START_DATE, begin_date);
        auto dist_begin_end = Date::ComputeDistance(begin_date, end_date) + 1;
        double daily = static_cast<double>(spending) / dist_begin_end;

        //std::cout << dist_start_begin << " " << dist_begin_end << std::endl;
        for (auto i = dist_start_begin; i < dist_begin_end + dist_start_begin; ++i) {
            spend_days[i] += daily;
        }
        return;
    }

    double ComputeIncome (const Date& begin_date, const Date& end_date){
        double income = 0;
        auto dist_start_begin = Date::ComputeDistance(START_DATE, begin_date);
        auto dist_begin_end = Date::ComputeDistance(begin_date, end_date) + 1;
        //std::cout << dist_start_begin << " " << dist_begin_end << std::endl;
        for (auto i = dist_start_begin; i < dist_begin_end + dist_start_begin; ++i) {
            income += earn_days[i];
            income -= spend_days[i];
        }
        
        return income;
    }

    void PayTax (const Date& begin_date, const Date& end_date, int rate){
        auto dist_start_begin = Date::ComputeDistance(START_DATE, begin_date);
        auto dist_begin_end = Date::ComputeDistance(begin_date, end_date) + 1;
        //std::cout << dist_start_begin << " " << dist_begin_end << std::endl;
        for (auto i = dist_start_begin; i < dist_begin_end + dist_start_begin; ++i) {
            earn_days[i] *= 1.0 - rate/100.0;
        }
        return;
    }

};