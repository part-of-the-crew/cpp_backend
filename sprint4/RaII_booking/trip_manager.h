#pragma once
#include "flight_provider.h"
#include "hotel_provider.h"

#include <string>
#include <vector>

class Trip {
private:
    HotelProvider& hp_;
    FlightProvider& fp_;
public:
    std::vector<FlightProvider::BookingId> flights;
    std::vector<HotelProvider::BookingId> hotels;

    Trip(HotelProvider& hp, FlightProvider& fp)
        : hp_{hp}
        , fp_{fp}
        {};
    Trip(const Trip&) = delete;
    Trip(Trip&&) = default;

    Trip& operator=(const Trip&) = delete;
    Trip& operator=(Trip&&) = default;
    void Cancel(){
        /*
        for (auto& id : hotels) {
            hotel_provider_.Cancel(id);
        }
        hotels.clear();
        for (auto& id : flights) {
            flight_provider_.Cancel(id);
        }
        flights.clear();
        */
        if (hotels.size() == flights.size()){
            hp_.Cancel(hotels.back());
            fp_.Cancel(flights.back());
            hotels.pop_back();
            flights.pop_back();
        }
    };
    ~Trip(){
        Cancel();
        if (hotels.size() != flights.size()){
            hp_.Cancel(hotels.back());
            fp_.Cancel(flights.back());
            fp_.Cancel(flights.back());
            hotels.pop_back();
            flights.pop_back();
        }
    };
};

class TripManager {
public:
    using BookingId = std::string;
    struct BookingData {
        std::string city_from;
        std::string city_to;
        std::string date_from;
        std::string date_to;
    };

    Trip Book([[maybe_unused]] const BookingData& data) {
        Trip trip(hotel_provider_, flight_provider_);
        {
            FlightProvider::BookingData flight_booking_data;
            trip.flights.push_back(flight_provider_.Book(flight_booking_data));
        }
        {
            HotelProvider::BookingData hotel_booking_data;
            trip.hotels.push_back(hotel_provider_.Book(hotel_booking_data));
        }
        {
            FlightProvider::BookingData flight_booking_data;
            trip.flights.push_back(flight_provider_.Book(flight_booking_data));
        }
        return trip;
    }

    void Cancel(Trip& trip) {
        trip.Cancel();
    }

private:
    HotelProvider hotel_provider_;
    FlightProvider flight_provider_;
};