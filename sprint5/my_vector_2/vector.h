#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <type_traits>
#include <memory>
#include <algorithm>

template <typename T>
class RawMemory {
public:
    RawMemory() = default;
    RawMemory(const RawMemory&) = delete;
    RawMemory& operator=(const RawMemory& other) = delete;
    RawMemory(RawMemory&& other) noexcept {
        if (this != &other) {
            Swap(other);
        }
    }
    RawMemory& operator=(RawMemory&& other) noexcept {
        if (this != &other) {
            Swap(other);
        }
        return *this;
    }
    explicit RawMemory(size_t capacity)
        : buffer_(Allocate(capacity))
        , capacity_(capacity) {
    }

    ~RawMemory() {
        Deallocate(buffer_);
    }

    T* operator+(size_t offset) noexcept {
        // Разрешается получать адрес ячейки памяти, следующей за последним элементом массива
        assert(offset <= capacity_);
        return buffer_ + offset;
    }

    const T* operator+(size_t offset) const noexcept {
        return const_cast<RawMemory&>(*this) + offset;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<RawMemory&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < capacity_);
        return buffer_[index];
    }

    void Swap(RawMemory& other) noexcept {
        std::swap(buffer_, other.buffer_);
        std::swap(capacity_, other.capacity_);
    }

    const T* GetAddress() const noexcept {
        return buffer_;
    }

    T* GetAddress() noexcept {
        return buffer_;
    }

    size_t Capacity() const {
        return capacity_;
    }

private:
    // Выделяет сырую память под n элементов и возвращает указатель на неё
    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    // Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
};

template <typename T>
class Vector {
public:

    using iterator = T*;
    using const_iterator = const T*;
    
    Vector() noexcept = default;

    explicit Vector(size_t size)
        : data_(size)
        , size_(size)  //
    {
        std::uninitialized_value_construct_n(data_.GetAddress(), size);
    }
    Vector(const Vector& other)
        : data_(other.size_)
        , size_(other.size_)  //
    {
        std::uninitialized_copy_n(other.data_.GetAddress(), size_, data_.GetAddress());
    };
    Vector& operator=(const Vector& rhs) {
        if (this == &rhs){
            return *this;
        }
        if (rhs.size_ > data_.Capacity()) {
            Vector rhs_copy(rhs);
            Swap(rhs_copy);
            return *this;
        }
        size_t size = std::min(rhs.size_, size_);
        std::copy_n(rhs.data_.GetAddress(), size, data_.GetAddress());
        if (rhs.size_ < size_){
            std::destroy_n(data_.GetAddress() + size, size_ - size);
        } else {
            std::uninitialized_copy_n(rhs.data_.GetAddress() + size, rhs.size_ - size, data_.GetAddress() + size);
        }
        size_ = rhs.size_;
        return *this;
    }

    Vector(Vector&& other) noexcept{
            Swap(other);
    }
    
    Vector& operator=(Vector&& other) noexcept{
        if (this != &other) {
            Swap(other);
        }
        return *this;
    }


    void Swap(Vector& other) noexcept{
        std::swap(other.size_, size_);
        other.data_.Swap(data_);
    }

    ~Vector() {
        std::destroy_n(data_.GetAddress(), size_);
    };

    size_t Size() const noexcept {
        return size_;
    };

    size_t Capacity() const noexcept {
        return data_.Capacity();
    };

    const T& operator[](size_t index) const noexcept {
        assert(index < size_);
        return const_cast<Vector&>(*this)[index];
    };

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    };

    void Reserve(size_t new_capacity) {
        if (new_capacity <= data_.Capacity()) {
            return;
        }
        RawMemory<T> new_data{new_capacity};
        Relocate(data_.GetAddress(), size_, new_data.GetAddress());
        std::destroy_n(data_.GetAddress(), size_);
        new_data.Swap(data_);
    };


    void Resize(size_t new_size){
        if (size_ >= new_size){
            std::destroy_n(data_.GetAddress() + new_size, size_ - new_size);
        } else {
            Reserve(new_size);
            std::uninitialized_value_construct_n(data_.GetAddress() + size_, new_size - size_);
        }
        size_ = new_size;
    };

    void PopBack() noexcept {
        assert(size_ > 0);
        std::destroy_at(data_.GetAddress() + --size_);
    }

    template<class... Args>
    T& EmplaceBack(Args&&... args){
        if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            std::construct_at(new_data + size_, std::forward<Args>(args)...);
            Relocate(data_.GetAddress(), size_, new_data.GetAddress());
            std::destroy_n(data_.GetAddress(), size_);
            new_data.Swap(data_);
        } else {
            std::construct_at(data_ + size_, std::forward<Args>(args)...);
        }
        ++size_;
        return (*this)[size_ - 1];
    };

    template<typename T0>
    void PushBack(T0&& value){
        EmplaceBack(std::forward<T0>(value));
    };

    template <typename... Args>
    iterator Emplace(const_iterator pos_, Args&&... args){
        assert(pos_ >= cbegin() && pos_ <= cend());
        auto pos = std::distance(cbegin(), pos_);
        if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            std::construct_at(new_data + pos, std::forward<Args>(args)...);
            Relocate(begin(), pos, new_data.GetAddress());
            Relocate(begin() + pos, size_ - pos, new_data.GetAddress() + pos + 1);
            std::destroy_n(data_.GetAddress(), size_);
            new_data.Swap(data_);
        } else {
            if (end() != pos_){
                T el(std::forward<Args>(args)...);
                std::construct_at(end(), std::move(*(end() - 1)));  
                if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                    std::move_backward(begin() + pos, end() - 1, end());
                } else {
                    try {
                        std::copy_backward(begin() + pos, end() - 1, end());
                    } catch (...){
                        std::destroy_n(end(), 1);
                    }
                }
                *(begin() + pos) = std::move(el);
            } else {
                std::construct_at(begin() + pos, std::forward<Args>(args)...);
            }
        }
        ++size_;
        return data_.GetAddress() + pos;
    }

    template<typename T0>
    iterator Insert(const_iterator pos_, T0&& value){
        return Emplace(pos_, std::forward<T0>(value));
    }

    iterator Erase(const_iterator pos_) /*noexcept(std::is_nothrow_move_assignable_v<T>)*/
    {
        assert(pos_ >= cbegin() && pos_ < cend());
        auto pos = std::distance(cbegin(), pos_);
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::move(begin() + pos + 1, end(), begin() + pos);
        } else {
            std::copy(begin() + pos + 1, end(), begin() + pos);
        }
        std::destroy_at(data_.GetAddress() + size_ - 1);
        --size_;
        return begin() + pos;
    }
    iterator begin() noexcept {
        return data_.GetAddress();
    }

    iterator end() noexcept {
        return data_.GetAddress() + size_;
    }

    const_iterator begin() const noexcept {
        return data_.GetAddress();
    }

    const_iterator end() const noexcept {
        return data_.GetAddress() + size_;
    }

    const_iterator cbegin() const noexcept {
        return data_.GetAddress();
    }

    const_iterator cend() const noexcept {
        return data_.GetAddress() + size_;
    }
private:
    RawMemory<T> data_;
    size_t size_ = 0;

    void Relocate(T* src, size_t count, T* dst) {
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(src, count, dst);
        } else {
            SafeUninitializedCopyN(src, count, dst);
        }
    }

    template <typename InputIt, typename Size, typename ForwardIt>
    ForwardIt SafeUninitializedCopyN(InputIt src, Size count, ForwardIt dst) {
        ForwardIt current = dst;
        try {
            for (; count > 0; --count, (void)++src, (void)++current) {
                std::construct_at(std::to_address(current), *src);
            }
            return current;
        } catch (...) {
            std::destroy(dst, current);  // destroy constructed elements
            throw;                       // rethrow the exception
        }
    }
};
