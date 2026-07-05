#include "unit_and_conversion_calculator.hpp"

namespace unit_and_conversion_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Unit and conversion calculator (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
