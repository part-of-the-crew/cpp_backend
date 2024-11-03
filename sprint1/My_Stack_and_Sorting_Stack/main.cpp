#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <numeric>

#include <random> 
using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Type>
class Stack {
public:
    void Push(const Type& element) {
        // напишите реализацию
        elements_.push_back(element);
    }
    void Pop() {
        elements_.pop_back();
        // напишите реализацию
    }
    const Type& Peek() const {
        // напишите реализацию
        return elements_.back();
    }
    Type& Peek() {
        // напишите реализацию
        return elements_.back();
    }
    void Print() const {
        // напишите реализацию
        //for (auto const& element: elements_)
        PrintRange(elements_.cbegin(), elements_.cend());
    }
    uint64_t Size() const {
        // напишите реализацию
        return elements_.size();
    }
    bool IsEmpty() const {
        // напишите реализацию
        return elements_.empty();
    }

private:
    vector<Type> elements_;
};

template <typename Type>
class StackMin {
public:
    void Push(const Type& element) {
    // напишите реализацию метода
        elements_.Push(element);
        if (min.IsEmpty() || element <= min.Peek())
            min.Push(element);
    }
    void Pop() {
    // напишите реализацию метода
        if (min.Peek() == elements_.Peek())
            min.Pop();
        elements_.Pop();
    }
    const Type& Peek() const {
        return elements_.Peek();
    }
    Type& Peek() {
        return elements_.Peek();
    }
    void Print() const {
        elements_.Print();
    }
    uint64_t Size() const {
        return elements_.Size();
    }
    bool IsEmpty() const {
        return elements_.IsEmpty();
    }
    const Type& PeekMin() const {
    // напишите реализацию метода
        return min.Peek();
    }
    Type& PeekMin() {
    // напишите реализацию метода
        return min.Peek();
    }
private:
    Stack<Type> elements_;
    Stack<Type> min;
    // возможно, здесь вам понадобится что-то изменить
};

template <typename Type>
class SortedSack {
public:
    void Push(const Type& element) {
    // напишите реализацию метода
        if (elements_.IsEmpty()){
            elements_.Push(element);
            return;
        }
        while (elements_.Peek() < element)
        {
            temp.Push(elements_.Peek());
            elements_.Pop();
            if (elements_.IsEmpty())
                break;
        }
        temp.Push(element);
        while (!temp.IsEmpty())
        {
            elements_.Push(temp.Peek());
            temp.Pop();
        }
    }
    void Pop() {
    // напишите реализацию метода
        elements_.Pop();
    }
    const Type& Peek() const {
        return elements_.Peek();
    }
    Type& Peek() {
        return elements_.Peek();
    }
    void Print() const {
        elements_.Print();
    }
    uint64_t Size() const {
        return elements_.Size();
    }
    bool IsEmpty() const {
        return elements_.IsEmpty();
    }
private:
    Stack<Type> elements_;
    Stack<Type> temp;
};

int main() {
    SortedSack<int> sack;
    vector<int> values(5);
    // заполняем вектор для тестирования нашего класса
    iota(values.begin(), values.end(), 1);
    // перемешиваем значения
    random_device rd;
    mt19937 g(rd());
    shuffle(values.begin(), values.end(), g);

    // заполняем класс и проверяем, что сортировка сохраняется после каждой вставки
    for (int i = 0; i < 5; ++i) {
        cout << "Вставляемый элемент = "s << values[i] << endl;
        sack.Push(values[i]);
        sack.Print();
    }
}
