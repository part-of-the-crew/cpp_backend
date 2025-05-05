#include "people.h"

#include <stdexcept>

using namespace std;


/*
Programmer::Programmer(const string& name, int age, Gender gender) {
    // Напишите тело конструктора
}
*/
const string& Person::GetName() const {
    return name_;
}

int Person::GetAge() const {
    return age_;
}

Gender Person::GetGender() const {
    return gender_;
}

void Programmer::AddProgrammingLanguage(ProgrammingLanguage language) {
    languages.insert(language);
}

bool Programmer::CanProgram(ProgrammingLanguage language) const {
    return languages.count(language);
}


void Worker::AddSpeciality(WorkerSpeciality speciality) {
    specialities.insert(speciality);
}

bool Worker::HasSpeciality(WorkerSpeciality speciality) const {
    return specialities.count(speciality);
}