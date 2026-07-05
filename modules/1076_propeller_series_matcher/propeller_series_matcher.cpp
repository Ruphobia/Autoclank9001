#include "propeller_series_matcher.hpp"

namespace propeller_series_matcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Propeller series matcher (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
