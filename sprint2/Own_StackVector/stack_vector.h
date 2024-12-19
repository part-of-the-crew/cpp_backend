#pragma once

#include <array>
#include <stdexcept>

template <typename T, size_t N>
class StackVector {
private:
/*
    template<typename ValueType>
    class Iterator {
        friend class OwnContainer;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = ValueType;
        using pointer = ValueType*;
        using reference = ValueType&;
        Iterator() : ptr_{ nullptr } {}
        Iterator(pointer ptr) : ptr_{ ptr } {}
        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        reference operator[](difference_type n) const { return *(ptr_ + n); }
        Iterator& operator++() {
            ++ptr_;
            return *this;
        }
        Iterator operator++(int) {
            Iterator temp = *this;
            ++ptr_;
            return temp;
        }
        Iterator& operator--() {
            --ptr_;
            return *this;
        }
        Iterator operator--(int) {
            Iterator temp = *this;
            --ptr_;
            return temp;
        }
        Iterator& operator+=(difference_type n) {
            ptr_ += n;
            return *this;
        }
        Iterator& operator-=(difference_type n) {
            ptr_ -= n;
            return *this;
        }
        friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
            return lhs.ptr_ == rhs.ptr_;
        }
        friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
            return !(lhs == rhs);
        }
        friend bool operator<(const Iterator& lhs, const Iterator& rhs) {
            return lhs.ptr_ < rhs.ptr_;
        }
        friend bool operator>(const Iterator& lhs, const Iterator& rhs) {
            return rhs < lhs;
        }
        friend bool operator<=(const Iterator& lhs, const Iterator& rhs) {
            return !(rhs < lhs);
        }
        friend bool operator>=(const Iterator& lhs, const Iterator& rhs) {
            return !(lhs < rhs);
        }
        friend Iterator operator+(const Iterator& it, difference_type n) {
            Iterator temp = it;
            temp += n;
            return temp;
        }
        friend Iterator operator+(difference_type n, const Iterator& it) {
            return it + n;
        }
        friend Iterator operator-(const Iterator& it, difference_type n) {
            Iterator temp = it;
            temp -= n;
            return temp;
        }
        friend difference_type operator-(const Iterator& lhs, const Iterator& rhs) {
            return lhs.ptr_ - rhs.ptr_;
        }
    private:
        pointer ptr_{ nullptr };
    };
*/
    std::array<T, N> data_;
    size_t size_;
public:
    explicit StackVector(size_t a_size = 0)
            : size_(a_size)
            {
                if (a_size > N) {
                    throw std::invalid_argument("StackVector capacity exceeded");
                }
            }

    T& operator[](size_t index) {
        if (index >= size_) {
            throw std::out_of_range("StackVector index out of range");
        }
        return data_[index];
    };
    const T& operator[](size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("StackVector index out of range");
        }
        return data_[index];
    };

    //using iterator = typename std::array<T, size_t N>::iterator;
    auto begin() {
        return data_.begin();
    };
    auto end() {
        return data_.begin() + size_;
    };
    auto begin() const {
        return data_.begin();
    };
    auto end() const {
        return data_.begin() + size_;
    };

    /*
    const T* end() const {
        return const_cast<const T*>(begin() + Size());
    }
    */

    size_t Size() const {
        return size_;
    };
    size_t Capacity() const {
        return N;
    };

    void PushBack(const T& value) {
        if (N == size_) {
            throw std::overflow_error("StackVector overflown");
        }
        data_[size_] = value;
        size_++;
    };
    T PopBack() {
        if (size_ == 0) {
            throw std::underflow_error("StackVector underflown");
        }
        size_--;
        return data_[size_];
    };

};
