#pragma once

#include <cstddef>
#include <utility>
#include <iterator>

template <typename T>
class ArrayPtr {
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    // Создаёт в куче массив из size элементов типа Type.
    // Если size == 0, поле raw_ptr_ должно быть равно nullptr
    explicit ArrayPtr(size_t size) : raw_ptr_(size > 0 ? new T[size]{} : nullptr){
       
    }

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    explicit ArrayPtr(T* raw_ptr) noexcept : raw_ptr_(raw_ptr)  {
        // Реализуйте конструктор самостоятельно
    }

    // Запрещаем копирование
    ArrayPtr(const ArrayPtr&) = delete;


    ~ArrayPtr() {
        delete[] raw_ptr_;  // освобождаем память, если указатель не nullptr
    }

    // Запрещаем присваивание
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    // New assignment operator for nullptr
    ArrayPtr& operator=(std::nullptr_t) noexcept {
        delete[] raw_ptr_;
        raw_ptr_ = nullptr;
        return *this;
    }

    //move operator
    ArrayPtr(ArrayPtr&& other) noexcept {
        //raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
        std::swap(raw_ptr_, other.raw_ptr_);
        delete [] other.raw_ptr_;
        other.raw_ptr_ = nullptr;
    }

    //move =
    ArrayPtr& operator=(ArrayPtr&& other) noexcept {
        //raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
        std::swap(raw_ptr_, other.raw_ptr_);
        delete [] other.raw_ptr_;
        other.raw_ptr_ = nullptr;
        return *this;
    }

    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] T* Release() noexcept {
        auto raw_ptr_copy(raw_ptr_);
        raw_ptr_ = nullptr;
        return raw_ptr_copy;
    }

    // Возвращает ссылку на элемент массива с индексом index
    T& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const T& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const {
        return raw_ptr_ != nullptr;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    T* Get() const noexcept {
        return raw_ptr_;
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }

private:
    T* raw_ptr_ = nullptr;
};
