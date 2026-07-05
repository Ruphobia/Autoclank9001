#include "band_plan_and_regulatory_checker.hpp"

namespace band_plan_and_regulatory_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Band plan and regulatory checker (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
