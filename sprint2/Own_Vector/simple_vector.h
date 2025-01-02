#pragma once

#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <cassert>
#include "array_ptr.h"

struct ReserveProxyObj {
    size_t capacity_;
    explicit ReserveProxyObj(size_t capacity) : capacity_(capacity) {}
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
};

template <typename T>
class SimpleVector {
    size_t size_ = 0;
    size_t capacity_ = 0;
    ArrayPtr<T> ar_;

public:
    using Iterator = T*;
    using ConstIterator = const T*;


    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size)
        : SimpleVector(size, T{})
    {}

    SimpleVector(size_t size, const T& value)
        : size_(size)
        , capacity_(size)
        , ar_(new T[size]) 
    {
        std::fill(ar_.Get(), ar_.Get() + size_, value);
    }
    SimpleVector(const ReserveProxyObj& reserve)
        : size_(0)
        , capacity_(reserve.capacity_)
        , ar_(ArrayPtr<T>(capacity_)) 
    {}

    SimpleVector(std::initializer_list<T> init)
        : SimpleVector(init.size()) 
    {
        std::copy(init.begin(), init.end(), ar_.Get());
    }

    SimpleVector(const SimpleVector& other)
        : size_(other.size_)
        , capacity_(other.capacity_)
        , ar_(ArrayPtr<T>(other.capacity_))
    {
        std::copy(other.begin(), other.end(), ar_.Get());
    }

    SimpleVector& operator=(const SimpleVector& other) {
        if (this != &other) {
            SimpleVector temp(other);
            std::swap(*this, temp);
        }
        return *this;
    }

    SimpleVector(SimpleVector&& other) noexcept {
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
        ar_ =  std::exchange(other.ar_, nullptr);
    };

    SimpleVector& operator=(SimpleVector&& other) noexcept {
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
        ar_ =  std::exchange(other.ar_, nullptr);
        return *this;
    };

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    T& operator[](size_t index) noexcept {
        return ar_[index];
    }

    const T& operator[](size_t index) const noexcept {
        return ar_[index];
    }

    T& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return ar_[index];
    }

    const T& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return ar_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
            return;
        }
        if (new_size > capacity_) {
            Reserve(new_size);
        }
        if (new_size > size_) {
            //std::uninitialized_fill(ar_.Get() + size_, ar_.Get() + new_size, T{});
            for (size_t i = size_; i < new_size; ++i) {
                new (ar_.Get() + i) T{};
            } 
        }
        size_ = new_size;
    }
    
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const T& item) {
        auto old_size = size_;
        if (capacity_ == size_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        size_++;
        ar_[old_size] = item;
    }

    void PushBack(T&& item) {
        auto old_size = size_;
        if (capacity_ == size_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        size_++;
        ar_[old_size] = std::move(item);
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const T& value) {
        auto it = std::distance(cbegin(), pos);
        if (size_ == capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        size_++;
        std::copy_backward(begin() + it, end(), end() + 1);
        ar_[it] = value;
        return begin() + it;
    }

    Iterator Insert(ConstIterator pos, T&& value) {
        auto it = std::distance(cbegin(), pos);
        if (size_ == capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        std::move_backward(begin() + it, begin() + size_, begin() + size_ + 1); // Adjust move range
        ar_[it] = std::move(value);
        size_++;
        return begin() + it;
    }

    Iterator Erase(ConstIterator pos) {
        auto it = std::distance(cbegin(), pos); // Get position index
        std::move(begin() + it + 1, end(), begin() + it); // Shift elements left
        --size_;
        return begin() + it; // Return iterator to the next element
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(size_ > 0);
        size_--;
    }

    void Reserve(size_t new_capacity) {
        if (capacity_ >= new_capacity)
            return;
        auto new_ar = ArrayPtr<T>(new_capacity);
        std::move(ar_.Get(), ar_.Get() + size_, new_ar.Get());
        ar_ = std::move(new_ar);
        capacity_ = new_capacity;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        std::swap(*this, other);
    }

    Iterator begin() noexcept {
        return ar_.Get();
    }

    Iterator end() noexcept {
        return ar_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return ar_.Get();
    }

    ConstIterator end() const noexcept {
        return ar_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return ar_.Get();
    }

    ConstIterator cend() const noexcept {
        return ar_.Get() + size_;
    }
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
} 

