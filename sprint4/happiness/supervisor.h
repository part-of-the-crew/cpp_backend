#pragma once

#include "people.h"

/*
    Надзиратель за уровнем удовлетворённости.
    Способен наблюдать за состоянием человека
    Если уровень удовлетворённости человека опустится ниже минимального уровня, Надзиратель
    побуждает человека танцевать до тех пор, пока уровень уровень удовлетворённости
    не станет больше или равен максимальному значению
*/
class SatisfactionSupervisor : public PersonObserver{
    int min_satisfaction_;
    int max_satisfaction_;
public:
    // Конструктор принимает значение нижнего и верхнего уровня удовлетворённости
    SatisfactionSupervisor(int min_satisfaction, int max_satisfaction)
        : min_satisfaction_{min_satisfaction},
        max_satisfaction_{max_satisfaction}  
    {}
    bool reacting_ = false;
    void OnSatisfactionChanged(Person& person, int old_value, int new_value) override {
        if (reacting_) return; // Prevent re-entrance
        if (new_value < min_satisfaction_ && new_value < old_value) {
            reacting_ = true;
            do {
                person.Dance();  // will trigger ChangeSatisfaction() and notify
            } while (person.GetSatisfaction() < max_satisfaction_);
            reacting_ = false;
        }
    }
    
};
