#include <cassert>
#include <cstddef>  // нужно для nullptr_t
#include <utility>  // нужно для swap

using namespace std;

// Реализуйте шаблон класса UniquePtr
template <typename T>
class UniquePtr {
private:
    T* raw_ptr_ = nullptr;
public:
    UniquePtr() = default;

    explicit UniquePtr(T* ptr) 
        :raw_ptr_{ptr}
        {};

    UniquePtr(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) {
        raw_ptr_ = other.raw_ptr_;
        other.Release();
    };

    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr& operator=(nullptr_t) {
        delete raw_ptr_;
        raw_ptr_ = nullptr;
        return *this;
    };
    UniquePtr& operator=(UniquePtr&& other) {
        if (this != &other) {
            Reset(other.raw_ptr_);
            other.raw_ptr_ = nullptr;
        }
        return *this;
    };

    ~UniquePtr() {
        if (raw_ptr_ != nullptr)
            delete raw_ptr_;
        raw_ptr_ = nullptr;
    };

    T& operator*() const {
        return *raw_ptr_;
    };
    T* operator->() const {
        return raw_ptr_;
    };
    T* Release() {
        auto raw_ptr_copy(raw_ptr_);
        raw_ptr_ = nullptr;
        return raw_ptr_copy;
    };
    void Reset(T* ptr) {
        auto old_ptr = raw_ptr_;
        /* assigns “ptr” to the stored pointer */
        if (old_ptr)
            delete old_ptr;
        raw_ptr_ = ptr;
    };
    void Swap(UniquePtr& other) {
        std::swap(raw_ptr_, other.raw_ptr_);
    };
    T* Get() const {
        return raw_ptr_;
    };
};

struct Item {
    static int counter;
    int value;
    Item(int v = 0)
        : value(v) 
    {
        ++counter;
    }
    Item(const Item& other)
        : value(other.value) 
    {
        ++counter;
    }
    ~Item() {
        --counter;
    }
};

int Item::counter = 0;

void TestLifetime() {
    Item::counter = 0;
    {
        UniquePtr<Item> ptr(new Item);
        assert(Item::counter == 1);

        ptr.Reset(new Item);
        assert(Item::counter == 1);
    }
    assert(Item::counter == 0);

    {
        UniquePtr<Item> ptr(new Item);
        assert(Item::counter == 1);

        auto rawPtr = ptr.Release();
        assert(Item::counter == 1);

        delete rawPtr;
        assert(Item::counter == 0);
    }
    assert(Item::counter == 0);
}

void TestGetters() {
    UniquePtr<Item> ptr(new Item(42));
    assert(ptr.Get()->value == 42);
    assert((*ptr).value == 42);
    assert(ptr->value == 42);
}

int main() {
    TestLifetime();
    TestGetters();
}