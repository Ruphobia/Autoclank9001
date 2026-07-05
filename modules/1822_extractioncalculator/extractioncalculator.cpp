#include "extractioncalculator.hpp"

namespace extractioncalculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ExtractionCalculator (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
