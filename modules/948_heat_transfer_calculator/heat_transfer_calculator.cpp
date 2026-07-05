#include "heat_transfer_calculator.hpp"

namespace heat_transfer_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Heat transfer calculator (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
