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
            other.capacity_ = 0;
        }
    }
    RawMemory& operator=(RawMemory&& other) noexcept {
        if (this != &other) {
            Swap(other);
            other.capacity_ = 0;
        }
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
        if (this != &rhs) {
            if (rhs.size_ > data_.Capacity()) {
                Vector rhs_copy(rhs);
                Swap(rhs_copy);
            } else {
                size_t i = 0;
                if (rhs.size_ < size_){
                    //std::copy(data_.GetAddress(), rhs.size_, rhs.data_.GetAddress());
                    for (; i < rhs.size_; i++){
                        data_[i] = rhs[i];
                    }
                    std::destroy_n(data_.GetAddress() + i, size_ - i);
                } else {
                    for (; i < size_; i++){
                        data_[i] = rhs[i];
                    }
                    std::uninitialized_copy_n(rhs.data_.GetAddress() + i, rhs.size_ - i, data_.GetAddress() + i);
                }
                size_ = rhs.size_;
            }
        }
        return *this;
    }

    Vector(Vector&& other) noexcept{
        if (this != &other) {
            Swap(other);
        }
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
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
        } else {
            std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
        }
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
        std::destroy_at(data_.GetAddress() + size_ - 1);
        if (size_ > 0)
            size_ -= 1;
    };

    template<typename T0>
    void PushBack(T0&& value){
        if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            std::construct_at(new_data + size_, std::forward<T0>(value));
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
            } else {
                std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
            }
            std::destroy_n(data_.GetAddress(), size_);
            new_data.Swap(data_);
        } else {
            std::construct_at(data_ + size_, std::forward<T0>(value));
        }
        ++size_;
    };

    template<class... Args>
    T& EmplaceBack(Args&&... args){
        if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            std::construct_at(new_data + size_, std::forward<Args>(args)...);
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
            } else {
                std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
            }
            std::destroy_n(data_.GetAddress(), size_);
            new_data.Swap(data_);
        } else {
            std::construct_at(data_ + size_, std::forward<Args>(args)...);
        }
        ++size_;
        return (*this)[size_ - 1];
    };

    template <typename... Args>
    iterator Emplace(const_iterator pos_, Args&&... args){
        auto pos = std::distance(cbegin(), pos_);
        if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            std::construct_at(new_data + pos, std::forward<Args>(args)...);
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(begin(), pos, new_data.GetAddress());
                std::uninitialized_move_n(begin() + pos, size_ - pos, new_data.GetAddress() + pos + 1);
            } else {
                std::uninitialized_copy_n(begin(), pos, new_data.GetAddress());
                std::uninitialized_copy_n(begin() + pos, size_ - pos, new_data.GetAddress() + pos + 1);
            }
            std::destroy_n(data_.GetAddress(), size_);
            new_data.Swap(data_);
        } else {
            // Shift existing elements one step to the right
            std::construct_at(end(), std::move(*(end() - 1)));
            std::move_backward(begin() + pos, end() - 1, end());
            
            // Construct new element directly in place
            std::construct_at(begin() + pos, std::forward<Args>(args)...);
        }
        ++size_;
        return data_.GetAddress() + pos;
    }

    iterator Erase(const_iterator pos_) /*noexcept(std::is_nothrow_move_assignable_v<T>)*/
    {
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

    template<typename T0>
    iterator Insert(const_iterator pos_, T0&& value){
        auto pos = std::distance(cbegin(), pos_);
        if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            std::construct_at(new_data + pos, std::forward<T0>(value));
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(begin(), pos, new_data.GetAddress());
                std::uninitialized_move_n(begin() + pos, size_ - pos, new_data.GetAddress() + pos + 1);
            } else {
                std::uninitialized_copy_n(begin(), pos, new_data.GetAddress());
                std::uninitialized_copy_n(begin() + pos, size_ - pos, new_data.GetAddress() + pos + 1);
            }
            std::destroy_n(data_.GetAddress(), size_);
            new_data.Swap(data_);
        } else {
            //auto el = std::forward<T0>(value);
            std::decay_t<T0> el(std::forward<T0>(value));
            std::construct_at(end(), std::forward<T0>(*(end() - 1)));
            std::move_backward(begin() + pos, end() - 2, end() - 1);
            (*this)[pos] = std::forward<T0>(el);
        }
        ++size_;
        return data_.GetAddress() + pos;
    }
    //iterator Insert(const_iterator pos, const T& value);
    //iterator Insert(const_iterator pos, T&& value);

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
};
