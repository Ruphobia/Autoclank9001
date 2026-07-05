#include "bom_and_cost_estimator.hpp"

namespace bom_and_cost_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: BOM and cost estimator (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
