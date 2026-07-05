#include "pharmacokinetics_simulator.hpp"

namespace pharmacokinetics_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pharmacokinetics Simulator (Medical, pharmacology, healthcare). Awaits wire-up.";
    return s;
}

}
