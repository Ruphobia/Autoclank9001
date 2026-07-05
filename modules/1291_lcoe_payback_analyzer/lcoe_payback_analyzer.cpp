#include "lcoe_payback_analyzer.hpp"

namespace lcoe_payback_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: LCOE & Payback Analyzer (Energy systems). Awaits wire-up.";
    return s;
}

}
