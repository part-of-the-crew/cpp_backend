#pragma once

#include <string>

class Person;

// Наблюдатель за состоянием человека.
class PersonObserver {
public:
    // Этот метод вызывается, когда меняется состояние удовлетворённости человека
    virtual void OnSatisfactionChanged(Person& /*person*/, int /*old_value*/, int /*new_value*/) {
        // Реализация метода базового класса ничего не делает
    }

protected:
    // Класс PersonObserver не предназначен для удаления напрямую
    virtual ~PersonObserver() = default;
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
    virtual void Dance( void ) {
        danceCount += 1;
        ChangeSatisfaction (1);
    }
    void ChangeSatisfaction( int delta) {
        int old_satisfaction = satisfaction_;
        satisfaction_ += delta;
        if (observer_ != nullptr)
            observer_->OnSatisfactionChanged(*this, old_satisfaction, satisfaction_);

    }
    int GetDanceCount() const {
        return danceCount;
    }

    // Прожить день. Реализация в базовом классе ничего не делает
    virtual void LiveADay() {
        // Подклассы могут переопределить этот метод
    }
    virtual ~Person() = default;

private:
    std::string name_;
    PersonObserver* observer_ = nullptr;
    int age_;

protected:
    int danceCount = 0;

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
        ChangeSatisfaction(-5);
    }
    void LiveADay() override {
        Work();
    }
    // Возвращает значение счётчика сделанной работы
    int GetWorkDone() const {
        return work_;
    }

    void Dance() override {
        if (GetAge() > 30 && GetAge() < 40){
            ChangeSatisfaction(2);
        } else {
            ChangeSatisfaction(1);
        }
        danceCount++;
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
        ChangeSatisfaction(-3);
    }

    void LiveADay() override {
        Study();
    }
    void Dance() override {
        ChangeSatisfaction(1);
        danceCount++;
    }
    // Возвращает уровень знаний
    int GetKnowledgeLevel() const {
        return knowledge;
    }
};