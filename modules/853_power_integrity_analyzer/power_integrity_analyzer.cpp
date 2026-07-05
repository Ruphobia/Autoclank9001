#include "power_integrity_analyzer.hpp"

namespace power_integrity_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Power integrity analyzer (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
