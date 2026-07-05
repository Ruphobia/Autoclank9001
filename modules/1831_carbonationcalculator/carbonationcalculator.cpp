#include "carbonationcalculator.hpp"

namespace carbonationcalculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CarbonationCalculator (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
