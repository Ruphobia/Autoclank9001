#include "orbital_propagator.hpp"

namespace orbital_propagator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Orbital Propagator (Aerospace and orbital mechanics). Awaits wire-up.";
    return s;
}

}
