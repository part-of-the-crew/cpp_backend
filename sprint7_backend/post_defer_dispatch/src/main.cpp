
#include <boost/asio.hpp>
#include <iostream>
#include <syncstream>
#include <thread>

namespace net = boost::asio;
namespace chrono = std::chrono;
using namespace std::literals;

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
}

void post() {
    using osync = std::osyncstream;

    net::io_context io{2};
    auto strand = net::make_strand(io);

    net::post(strand, [] { osync(std::cout) << "Cat, "; });

    net::post(strand, [] { osync(std::cout) << "Fish, "; });

    net::post(io, [] { osync(std::cout) << "Dog, "; });

    RunWorkers(2, [&io] { io.run(); });

    std::cout << std::endl;
}

void dispatch() {
    net::io_context io;

    net::post(io, [&io] {  // (1)
        std::cout << 'A';
        net::post(io, [] {  // (2)
            std::cout << 'B';
        });
        std::cout << 'C';
    });

    net::dispatch(io, [&io] {  // (3)
        std::cout << 'D';
        net::post(io, [] {  // (4)
            std::cout << 'E';
        });
        net::defer(io, [] {  // (5)
            std::cout << 'F';
        });
        net::dispatch(io, [] {  // (6)
            std::cout << 'G';
        });
        std::cout << 'H';
    });

    std::cout << 'I';
    io.run();
    std::cout << 'J';
}

int main() {
    dispatch();
}
