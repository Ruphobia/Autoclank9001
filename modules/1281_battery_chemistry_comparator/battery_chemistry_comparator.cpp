#include "battery_chemistry_comparator.hpp"

namespace battery_chemistry_comparator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Battery Chemistry Comparator (Energy systems). Awaits wire-up.";
    return s;
}

}
