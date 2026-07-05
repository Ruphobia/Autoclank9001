#include "weight_and_balance_calculator.hpp"

namespace weight_and_balance_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Weight and balance calculator (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
