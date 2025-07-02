#include <cassert>
#include <functional>
#include <string>
#include <memory>

using namespace std;

template <typename T>
class LazyValue {
public:
    explicit LazyValue(function<T()> init)
        : init_(move(init))
        {};

    bool HasValue() const {
        return pvalue != nullptr ? true : false; 
    };
    const T& Get() const {
        if (!HasValue())
            pvalue = std::make_unique<T>(init_());
        return *pvalue;
    };

private:
    std::function<T()> init_;
    mutable std::unique_ptr<T> pvalue;
};

void UseExample() {
    const string big_string = "Giant amounts of memory"s;

    LazyValue<string> lazy_string([&big_string] {
        return big_string;
    });

    assert(!lazy_string.HasValue());
    assert(lazy_string.Get() == big_string);
    assert(lazy_string.Get() == big_string);
}

void TestInitializerIsntCalled() {
    bool called = false;

    {
        LazyValue<int> lazy_int([&called] {
            called = true;
            return 0;
        });
    }
    assert(!called);
}

int main() {
    UseExample();
    TestInitializerIsntCalled();
}