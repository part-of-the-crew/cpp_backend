#pragma once

namespace raii {

template <typename Provider>
class Booking {
private:
    using BookingId = typename Provider::BookingId;

    Provider* provider_;
    int counter_;

public:
    Booking(Provider* p, int counter)
        : provider_{p}
        , counter_{counter}
        {};
    Booking(const Booking&) = delete;
    Booking(Booking&& other) = default;
    Booking& operator=(const Booking&) = delete;
    Booking& operator=(Booking&& other) = default;
    ~Booking(){
        provider_->CancelOrComplete(*this);
    };

    // Эта функция не требуется в тестах, но в реальной программе она может быть нужна
    BookingId GetId() const {
        return  {provider_, counter_};
    };   
};

}//namespace raii