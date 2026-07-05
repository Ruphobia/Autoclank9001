#include "decline_curve_analyzer.hpp"

namespace decline_curve_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Decline curve analyzer (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
