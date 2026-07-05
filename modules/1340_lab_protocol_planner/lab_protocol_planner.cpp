#include "lab_protocol_planner.hpp"

namespace lab_protocol_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lab Protocol Planner (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
