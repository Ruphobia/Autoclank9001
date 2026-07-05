#include "rf_design_suite.hpp"

namespace rf_design_suite {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: RF design suite (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
