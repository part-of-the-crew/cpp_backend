#include "libstat.h"

#include <cassert>
#include <cmath>
#include <sstream>

using namespace std;

namespace statistics {
namespace tests::detail {
template <typename T>
std::string GetPrinterValue(statistics::AggregPrinter<T>& printer) {
    std::ostringstream out;
    printer.Print(out);

    return std::move(out).str();
}
}
void tests::AggregSum() {
    aggregations::Sum aggreg;
    assert(*aggreg.Get() == 0);

    aggreg.PutValue(10.);
    aggreg.PutValue(20.);
    aggreg.PutValue(-40.);

    assert(*aggreg.Get() == -10.);
}

void tests::AggregMax() {
    aggregations::Max aggreg;
    assert(!aggreg.Get());

    aggreg.PutValue(10.);
    aggreg.PutValue(20.);
    aggreg.PutValue(-40.);

    assert(*aggreg.Get() == 20.);
}

void tests::AggregMean() {
    aggregations::Mean aggreg;
    assert(!aggreg.Get());

    aggreg.PutValue(10.);
    aggreg.PutValue(20.);
    aggreg.PutValue(-40.);
    aggreg.PutValue(30.);

    assert(*aggreg.Get() == 5.);
}

void tests::AggregStandardDeviation() {
    aggregations::StandardDeviation aggreg;
    assert(!aggreg.Get());

    aggreg.PutValue(10.);
    aggreg.PutValue(10.);
    aggreg.PutValue(10.);
    aggreg.PutValue(10.);

    assert(std::abs(*aggreg.Get()) < 1e-5);

    aggreg.PutValue(20.);
    aggreg.PutValue(20.);
    aggreg.PutValue(20.);
    aggreg.PutValue(20.);

    assert(std::abs(*aggreg.Get() - 5.) < 1e-5);
}

void tests::AggregMode() {
    aggregations::Mode aggreg;
    assert(!aggreg.Get());

    aggreg.PutValue(1.1);
    aggreg.PutValue(0.9);
    aggreg.PutValue(2.1);
    aggreg.PutValue(2.2);
    aggreg.PutValue(2.1);
    aggreg.PutValue(-1.0);
    aggreg.PutValue(3.0);
    aggreg.PutValue(3.0);
    aggreg.PutValue(1000.);

    assert(std::round(*aggreg.Get()) == 2.);
}

void tests::AggregPrinter() {
    statistics::AggregPrinter<aggregations::Max> printer;

    assert(tests::detail::GetPrinterValue(printer) == "max is undefined\n"s);
    printer.PutValue(10.);
    printer.PutValue(20.);
    printer.PutValue(-40.);

    std::ostringstream out;
    out << 20.;

    assert(tests::detail::GetPrinterValue(printer) == "max is "s + out.str() + "\n"s);
}
} // namespace statistics::aggregations