#include "performance_calculator.hpp"

namespace performance_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Performance calculator (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
