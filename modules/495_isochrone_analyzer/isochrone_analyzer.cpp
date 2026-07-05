#include "isochrone_analyzer.hpp"

namespace isochrone_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Isochrone analyzer (Logistics, supply chain, operations research). Awaits wire-up.";
    return s;
}

}
