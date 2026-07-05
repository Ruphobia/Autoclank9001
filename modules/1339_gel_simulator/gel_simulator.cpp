#include "gel_simulator.hpp"

namespace gel_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gel Simulator (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
