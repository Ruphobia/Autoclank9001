#include "dilutioncalculator.hpp"

namespace dilutioncalculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: DilutionCalculator (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
