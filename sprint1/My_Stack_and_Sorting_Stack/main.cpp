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

template <typename T>
class Stack {
public:
    void Push(const T& element) {
        // напишите реализацию
        elements_.push_back(element);
    }
    void Pop() {
        elements_.pop_back();
        // напишите реализацию
    }
    const T& Peek() const {
        // напишите реализацию
        return elements_.back();
    }
    T& Peek() {
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
    vector<T> elements_;
};

template <typename T>
class StackMin {
public:
    void Push(const T& element) {
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
    const T& Peek() const {
        return elements_.Peek();
    }
    T& Peek() {
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
    const T& PeekMin() const {
    // напишите реализацию метода
        return min.Peek();
    }
    T& PeekMin() {
    // напишите реализацию метода
        return min.Peek();
    }
private:
    Stack<T> elements_;
    Stack<T> min;
    // возможно, здесь вам понадобится что-то изменить
};

template <typename T>
class SortedSack {
public:
    void Push(const T& element) {
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
    const T& Peek() const {
        return elements_.Peek();
    }
    T& Peek() {
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
    Stack<T> elements_;
    Stack<T> temp;
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
