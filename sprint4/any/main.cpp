#include <iostream>
#include <string>
#include <string_view>
#include <memory>

using namespace std;

class AnyStorageBase {
public:
    virtual ~AnyStorageBase() = default;
    virtual void Print(ostream& out) const = 0;
};

template <typename T>
class AnyStorage: public AnyStorageBase {
public:
    // конструктор AnyStorage, принимающий T универсальным образом
    //explicit AnyStorage(const T& value) : data_(value) {}
    //explicit AnyStorage(T&& value) : data_(std::move(value)) {}
    template<typename U>
    explicit AnyStorage(U&& value) : data_(std::forward<U>(value)) {}
    void Print(ostream& out) const override{
        out << data_;
    }
    ~AnyStorage() {};

private:
    T data_;
}; 

class Any {
    unique_ptr<AnyStorageBase> ptr_;
public:
    template <typename S>
    Any (S&& obj){
        //using Initial = std::remove_reference_t<S>;
        ptr_ = std::make_unique<AnyStorage<std::remove_reference_t<S>>>(std::forward<S>(obj));
    }
    void Print(std::ostream& out) const {
        ptr_->Print(out);
    }
};

class Dumper {
public:
    Dumper() {
        std::cout << "construct"sv << std::endl;
    }
    ~Dumper() {
        std::cout << "destruct"sv << std::endl;
    }
    Dumper(const Dumper&) {
        std::cout << "copy"sv << std::endl;
    }
    Dumper(Dumper&&) {
        std::cout << "move"sv << std::endl;
    }
    Dumper& operator=(const Dumper&) {
        std::cout << "= copy"sv << std::endl;
        return *this;
    }
    Dumper& operator=(Dumper&&) {
        std::cout << "= move"sv << std::endl;
        return *this;
    }
};

ostream& operator<<(ostream& out, const Any& arg) {
    arg.Print(out);
    return out;
}

ostream& operator<<(ostream& out, const Dumper&) {
    return out;
}

int main() {
    Any any_int(42);
    Any any_string("abc"s);

    // операция вывода Any в поток определена чуть выше в примере
    cout << any_int << endl << any_string << endl;

    Any any_dumper_temp{Dumper{}};

    Dumper auto_dumper;
    Any any_dumper_auto(auto_dumper);
}