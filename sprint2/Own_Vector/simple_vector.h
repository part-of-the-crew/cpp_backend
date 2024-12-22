#pragma once

#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <utility>

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
    std::unique_ptr<T[]> ar_;

public:
    using Iterator = T*;
    using ConstIterator = const T*;


    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size)
        : size_(size)
        , capacity_(size)
        , ar_(std::make_unique<T[]>(size)) 
    {}

    SimpleVector(size_t size, const T& value)
        : SimpleVector(size) 
    {
        std::fill(ar_.get(), ar_.get() + size_, value);
    }
    SimpleVector(const ReserveProxyObj& reserve)
        : size_(0)
        , capacity_(reserve.capacity_)
        , ar_(std::make_unique<T[]>(capacity_)) 
    {}

    SimpleVector(std::initializer_list<T> init)
        : SimpleVector(init.size()) 
    {
        std::copy(init.begin(), init.end(), ar_.get());
    }

    SimpleVector(const SimpleVector& other)
        : size_(other.size_), capacity_(other.capacity_),
          ar_(std::make_unique<T[]>(other.capacity_)) 
    {
        std::copy(other.begin(), other.end(), ar_.get());
    }

    SimpleVector& operator=(const SimpleVector& other) {
        if (this != &other) {
            SimpleVector temp(other);
            std::swap(*this, temp);
        }
        return *this;
    }

    SimpleVector(SimpleVector&& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(ar_, other.ar_);
    };
    SimpleVector& operator=(SimpleVector&& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(ar_, other.ar_);
        return *this;
    };

    size_t GetSize() const noexcept { return size_; }
    size_t GetCapacity() const noexcept { return capacity_; }
    bool IsEmpty() const noexcept { return size_ == 0; }

    T& operator[](size_t index) noexcept { return ar_[index]; }
    const T& operator[](size_t index) const noexcept { return ar_[index]; }

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

    void Clear() noexcept { size_ = 0; }

    size_t Allocate(size_t new_capacity) {
        if (new_capacity == 0)
            new_capacity = 1;
        auto new_ar = std::make_unique<T[]>(new_capacity);
        std::move(ar_.get(), ar_.get() + size_, new_ar.get());
        ar_ = std::move(new_ar);
        capacity_ = new_capacity;
        return new_capacity;
    }

    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
            return;
        }
        if (new_size > capacity_) {
            Allocate(new_size);
        }
        if (new_size > size_) {
            //std::uninitialized_fill(ar_.get() + size_, ar_.get() + new_size, T{});
            for (size_t i = size_; i < new_size; ++i) {
                new (ar_.get() + i) T{};
            } 
        }
        size_ = new_size;
    }
    
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const T& item) {
        auto old_size = size_;
        if (capacity_ == size_) {
            Allocate(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        size_++;
        ar_[old_size] = item;
    }

    void PushBack(T&& item) {
        auto old_size = size_;
        if (capacity_ == size_) {
            Allocate(capacity_ == 0 ? 1 : capacity_ * 2);
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
            Allocate(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        size_++; // Increase size for correct calculation of new position
        std::copy_backward(begin() + it, end(), end() + 1); // Correctly shift elements
        ar_[it] = value;
        return begin() + it;
    }

    Iterator Insert(ConstIterator pos, T&& value) {
        auto it = std::distance(cbegin(), pos);
        if (size_ == capacity_) {
            Allocate(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        size_++; // Increase size for correct calculation of new position
        std::move_backward(begin() + it, end(), end() + 1); // Correctly shift elements
        ar_[it] = std::move(value);
        return begin() + it;
    }

    Iterator Erase(ConstIterator pos) {
        auto it = std::distance(cbegin(), pos); // Get position index
        std::move(begin() + it + 1, end(), begin() + it); // Shift elements left
        --size_; // Decrease size
        return begin() + it; // Return iterator to the next element
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (size_ > 0)
            size_--;
    }
    void Reserve(size_t new_capacity) {
        if (capacity_ >= new_capacity)
            return;
        Allocate(new_capacity);
    }
    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        std::swap(*this, other);
    }
    Iterator begin() noexcept { return ar_.get(); }
    Iterator end() noexcept { return ar_.get() + size_; }
    ConstIterator begin() const noexcept { return ar_.get(); }
    ConstIterator end() const noexcept { return ar_.get() + size_; }
    ConstIterator cbegin() const noexcept { return ar_.get(); }
    ConstIterator cend() const noexcept { return ar_.get() + size_; }
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

