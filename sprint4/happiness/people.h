#pragma once

#include <string>

class Person;

// Наблюдатель за состоянием человека.
class PersonObserver {
public:
    // Этот метод вызывается, когда меняется состояние удовлетворённости человека
    void OnSatisfactionChanged(Person& /*person*/, int /*old_value*/, int /*new_value*/) {
        // Реализация метода базового класса ничего не делает
    }

protected:
    // Класс PersonObserver не предназначен для удаления напрямую
    ~PersonObserver() = default;
};

/*
    Человек.
    При изменении уровня удовлетворённости уведомляет
    связанного с ним наблюдателя
*/
class Person {
public:
    Person(const std::string& name, int age) : name_{name}, age_{age} {}

    int GetSatisfaction() const {
        return satisfaction_;
    }

    const std::string& GetName() const {
        return name_;
    }

    // «Привязывает» наблюдателя к человеку. Привязанный наблюдатель
    // уведомляется об изменении уровня удовлетворённости человека
    // Новый наблюдатель заменяет собой ранее привязанного
    // Если передать nullptr в качестве наблюдателя, это эквивалентно отсутствию наблюдателя
    void SetObserver(PersonObserver* observer) {
        observer_ = observer;
        
    }

    int GetAge() const {
        return age_;
    }

    // Увеличивает на 1 количество походов на танцы
    // Увеличивает удовлетворённость на 1
    virtual void Dance() {
        danceCount += 1;
        observer_->OnSatisfactionChanged(*this, satisfaction_, satisfaction_ + 1);
        satisfaction_++;
    }
    int ChangeSatisfaction( int satisfaction) {
        observer_->OnSatisfactionChanged(*this, satisfaction_, satisfaction_ + 1);
        return danceCount;
    }
    int GetDanceCount() const {
        return danceCount;
    }

    // Прожить день. Реализация в базовом классе ничего не делает
    virtual void LiveADay() {
        // Подклассы могут переопределить этот метод
    }

private:
    std::string name_;
    PersonObserver* observer_ = nullptr;
    int age_;
    int danceCount = 0;
protected:
    int satisfaction_ = 100;
};

// Рабочий.
// День рабочего проходит за работой
class Worker : public Person {
    int work_ = 0;
public:
    Worker(const std::string& name, int age): Person(name, age) {}

    // Рабочий старше 30 лет и младше 40 за танец получает 2 единицы удовлетворённости вместо 1

    // День рабочего проходит за работой

    // Увеличивает счётчик сделанной работы на 1, уменьшает удовлетворённость на 5
    void Work() {
        work_++;
        satisfaction_ -= 5;
    }

    // Возвращает значение счётчика сделанной работы
    int GetWorkDone() const {
        return work_;
    }

    void Dance() override {
        if (GetAge() > 30 && GetAge() < 40){
            work_ += 2;
        } else {
            work_ += 1;
        }
    }
};

// Студент.
// День студента проходит за учёбой
class Student : public Person {
    int knowledge = 0;
public:
    Student(const std::string& name, int age): Person(name, age) {}

    // День студента проходит за учёбой

    // Учёба увеличивает уровень знаний на 1, уменьшает уровень удовлетворённости на 3
    void Study() {
        knowledge++;
        satisfaction_ -= 3;
    }

    // Возвращает уровень знаний
    int GetKnowledgeLevel() const {
        return knowledge;
    }
};