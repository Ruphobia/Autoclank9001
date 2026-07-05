#include "rigid_body_simulator.hpp"

namespace rigid_body_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rigid-body simulator (Physics and math). Awaits wire-up.";
    return s;
}

}
