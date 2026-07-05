#include "cipw_norm_calculator.hpp"

namespace cipw_norm_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CIPW Norm Calculator (Geology, GIS, and earth sciences). Awaits wire-up.";
    return s;
}

}
