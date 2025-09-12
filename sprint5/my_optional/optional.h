#include <stdexcept>
#include <utility>

// Исключение этого типа должно генерироватся при обращении к пустому optional
class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() = default;

    Optional(const T& value) {
        new (&data_) T(value);
        is_initialized_ = true;
    }

    Optional(T&& value) {
        new (&data_) T(std::move(value));
        is_initialized_ = true;
    }

    Optional(const Optional& other) {
        if (other.is_initialized_) {
            new (&data_) T(*other.ptr());
            is_initialized_ = true;
        }
    }

    Optional(Optional&& other) {
        if (this == &other) return ;
        if (!other.is_initialized_) {
            Reset();
        } else if (is_initialized_) {
            *ptr() = std::move(*other.ptr());
        } else {
            new (&data_) T(std::move(*other.ptr()));
            is_initialized_ = true;
        }
    }

    Optional& operator=(const T& value) {
        if (is_initialized_) {
            *ptr() = value;               // assign into existing object
        } else {
            new (&data_) T(value);
            is_initialized_ = true;
        }
        return *this;
    }

    Optional& operator=(T&& value) {
        if (is_initialized_) {
            *ptr() = std::move(value);
        } else {
            new (&data_) T(std::move(value));
            is_initialized_ = true;
        }
        return *this;
    }

    Optional& operator=(const Optional& other) {
        if (this == &other) return *this;
        if (!other.is_initialized_) {
            Reset();
        } else if (is_initialized_) {
            *ptr() = *other.ptr();
        } else {
            new (&data_) T(*other.ptr());
            is_initialized_ = true;
        }
        return *this;
    }

    Optional& operator=(Optional&& other) {
        if (this == &other) return *this;
        if (!other.is_initialized_) {
            Reset();
        } else if (is_initialized_) {
            *ptr() = std::move(*other.ptr());
        } else {
            new (&data_) T(std::move(*other.ptr()));
            is_initialized_ = true;
        }
        return *this;
    }

    ~Optional() { Reset(); }

    bool HasValue() const { return is_initialized_; }
    explicit operator bool() const { return is_initialized_; }

    //Реализуйте в классе Optional перегрузку оператора * и метода Value для rvalue-ссылок, 
    //чтобы при вызове у временных объектов они возвращали rvalue-ссылку на содержимое Optional.


    T&& Value() && {
        if (!is_initialized_) throw BadOptionalAccess{};
        return std::move(*ptr());
    }

    T& Value() & {
        if (!is_initialized_) throw BadOptionalAccess{};
        return *ptr();
    }
    const T& Value() const& {
        if (!is_initialized_) throw BadOptionalAccess{};
        return *ptr();
    }
    T&& operator*() && { return std::move(*ptr()); }

    T& operator*() & { return *ptr(); }

    const T& operator*() const& { return *ptr(); }

    T* operator->() { return ptr(); }
    const T* operator->() const { return ptr(); }

    void Reset() {
        if (is_initialized_) {
            ptr()->~T();
            is_initialized_ = false;
        }
    }
    template<class... Args>
    T& Emplace(Args&&... args){
        Reset();
        new (&data_) T(std::forward<Args>(args)...);
        is_initialized_ = true;
        return *ptr();
    }
private:
    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;

    T* ptr() { return reinterpret_cast<T*>(&data_); }
    const T* ptr() const { return reinterpret_cast<const T*>(&data_); }
};