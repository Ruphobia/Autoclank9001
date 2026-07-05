#include "propagation_predictor.hpp"

namespace propagation_predictor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Propagation predictor (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
