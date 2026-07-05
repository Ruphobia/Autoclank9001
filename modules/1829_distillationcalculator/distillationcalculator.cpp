#include "distillationcalculator.hpp"

namespace distillationcalculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: DistillationCalculator (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
