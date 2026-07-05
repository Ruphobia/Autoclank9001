#include "earthwork_calculator.hpp"

namespace earthwork_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Earthwork Calculator (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
